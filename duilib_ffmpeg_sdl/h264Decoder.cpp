#include "h264Decoder.h"
#include <iostream>
#include <string>
#include <Windows.h>

//#include "opencv2/opencv.hpp"

#ifdef __cplusplus

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h" 
#include "libswscale/swscale.h" 
#include "libavutil/avutil.h" 

#include "SDL.h"
};

#endif

#define INBUF_SIZE 1024
bool pause = false;
//cv::VideoWriter *writer;

static std::string TCHAR2STRING(TCHAR *STR) {
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);

	char* chRtn = new char[iLen * sizeof(char)];

	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);

	std::string str(chRtn);

	return str;
}

static std::string GetFileName() {
	OPENFILENAME ofn;      // 公共对话框结构。     
	TCHAR szFile[MAX_PATH]; // 保存获取文件名称的缓冲区。               
							// 初始化选择文件对话框。     
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = (LPCWSTR)"All(*.*)\0*.*\0Text(*.h264)\0*.h264\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	//ofn.lpTemplateName =  MAKEINTRESOURCE(ID_TEMP_DIALOG);    
	// 显示打开选择文件对话框。
	if (GetOpenFileName(&ofn))
	{
		//显示选择的文件。 
		std::cout << szFile << std::endl;
		//OutputDebugString(szFile);    
		//OutputDebugString((LPCWSTR)"\r\n");
	}

	std::string res = TCHAR2STRING(szFile);

	return res;
}

SDL_Rect m_sdlRect;
SDL_Renderer *m_psdlRenderer;
SDL_Texture *m_psdlTexture;

static bool InitSDL(HWND hWnd) {
	SDL_Window *screen;
	int screen_w = 0, screen_h = 0;

	char sdl_var[64];
	sprintf(sdl_var, "SDL_WINDOWID=%d", hWnd);//主窗口句柄   


	//SDL---------------------------
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		char szLog[1024] = { 0 };
		sprintf_s(szLog, sizeof(szLog), "Could not initialize SDL - %s\n", SDL_GetError());
		OutputDebugStringA(szLog);
		return false;
	}

	screen_w = GetSystemMetrics(SM_CXSCREEN); //m_pCodecCtx->width;
	screen_h = GetSystemMetrics(SM_CYSCREEN);// m_pCodecCtx->height;

	//SDL 2.0 Support for multiple windows
	screen = SDL_CreateWindowFrom(hWnd);

	if (!screen)
	{
		return false;
	}

	m_psdlRenderer = SDL_CreateRenderer(screen, -1, 0);
	m_psdlTexture = SDL_CreateTexture(m_psdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, screen_w, screen_h);

	m_sdlRect.x = 0;
	m_sdlRect.y = 0;
	m_sdlRect.w = screen_w;
	m_sdlRect.h = screen_h;
	//SDL End----------------------

	return true;
}

static void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,
	const char *filename)
{
	char buf[1024];
	int ret;

	ret = avcodec_send_packet(dec_ctx, pkt);
	if (ret < 0) {
		fprintf(stderr, "Error sending a packet for decoding\n");
		exit(1);
	}

	while (ret >= 0) {
		ret = avcodec_receive_frame(dec_ctx, frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) {
			fprintf(stderr, "Error during decoding\n");
			exit(1);
		}

		printf("saving frame %3d\n", dec_ctx->frame_number);
		fflush(stdout);

		/* the picture is allocated by the decoder. no need to
		free it */
		snprintf(buf, sizeof(buf), "%s-%d", filename, dec_ctx->frame_number);

		int width = frame->width, height = frame->height;

		int nRes = SDL_UpdateYUVTexture(m_psdlTexture, &m_sdlRect,
			frame->data[0], frame->linesize[0],
			frame->data[1], frame->linesize[1],
			frame->data[2], frame->linesize[2]);

		SDL_RenderClear(m_psdlRenderer);
		SDL_RenderCopy(m_psdlRenderer, m_psdlTexture, NULL, &m_sdlRect);
		SDL_RenderPresent(m_psdlRenderer);
		//SDL End-----------------------
		//Delay 1ms
		SDL_Delay(30);

		//cv::Mat tmp_img = cv::Mat::zeros(height * 3 / 2, width, CV_8UC1);
		//memcpy(tmp_img.data, frame->data[0], width*height);
		//memcpy(tmp_img.data + width * height, frame->data[1], width*height / 4);
		//memcpy(tmp_img.data + width * height * 5 / 4, frame->data[2], width*height / 4);

		////cv::imshow("", tmp_img);
		//cv::Mat bgr;
		//cv::cvtColor(tmp_img, bgr, CV_YUV2BGR_I420);
		//int si = sizeof(bgr.data);
		//std::cout << "size : " << si << std::endl;
		////if (!si)
		////cv::imwrite("test.jpg", bgr);
		////if (si > 100)
		////writer->write(bgr);
		//cv::imshow("Video", bgr);
		//int key = cv::waitKey(10);
		//if (key == 's') {
		//	if (pause)
		//		cv::waitKey();
		//	else pause = !pause;
		//}

		//pgm_save(frame->data[0], frame->linesize[0],frame->width, frame->height, buf);
	}
}

int decodeFile(HWND pParentWnd)
{
	std::string filename, outfilename;
	const AVCodec *codec;
	AVCodecParserContext *parser;
	AVCodecContext *c = NULL;
	FILE *f;
	AVFrame *frame;
	uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
	uint8_t *data;
	size_t   data_size;
	int ret;
	AVPacket *pkt;

	InitSDL(pParentWnd);
	/*cv::namedWindow("Video", CV_WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle("Video");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, pParentWnd);
	::ShowWindow(hParent, SW_HIDE);
*/
	filename = "E:\\work\\resource\\common.h264";//GetFileName().c_str();
#ifdef FILE_OPERATION	
	//	if (argc <= 2) {
	//		fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
	//		exit(0);
	//	}
		//filename = argv[1];
		//outfilename = argv[2];
		//filename = "out.h264";
	std::cout << "0:" << filename << std::endl;
	outfilename = filename.substr(0, filename.find_last_of("\\")) + "\\decoded.avi";
	//CV_FOURCC('M', 'J', 'P', 'G')
	writer = new cv::VideoWriter();
	int ret1 = writer->open(outfilename, -1, 30.0, cv::Size(1280, 720));
	//int ret1 = writer->open(outfilename, CV_FOURCC('D', 'I', 'V', '3'), 30.0, cv::Size(1280, 720));

	if (!writer->isOpened()) {
		std::cout << "No open";
	}
#endif

	pkt = av_packet_alloc();
	if (!pkt)
		exit(1);

	/* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
	memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

	/* find the MPEG-1 video decoder */
	codec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!codec) {
		fprintf(stderr, "Codec not found\n");
		exit(1);
	}
	std::cout << "1:" << filename << std::endl;

	parser = av_parser_init(codec->id);
	if (!parser) {
		fprintf(stderr, "parser not found\n");
		exit(1);
	}
	std::cout << "2:" << filename << std::endl;

	c = avcodec_alloc_context3(codec);
	if (!c) {
		fprintf(stderr, "Could not allocate video codec context\n");
		exit(1);
	}

	/* For some codecs, such as msmpeg4 and mpeg4, width and height
	MUST be initialized there because this information is not
	available in the bitstream. */
	std::cout << "3:" << filename << std::endl;

	/* open it */
	if (avcodec_open2(c, codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		exit(1);
	}
	std::cout << "4:" << filename << std::endl;

	//std::cout << filename << std::endl;
	f = fopen(filename.c_str(), "rb");
	if (!f) {
		fprintf(stderr, "Could not open %s\n", filename);
		exit(1);
	}

	frame = av_frame_alloc();
	if (!frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		exit(1);
	}

	while (!feof(f)) {
		/* read raw data from the input file */
		data_size = fread(inbuf, 1, INBUF_SIZE, f);
		if (!data_size)
			break;

		/* use the parser to split the data into frames */
		data = inbuf;
		while (data_size > 0) {
			ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
				data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
			if (ret < 0) {
				fprintf(stderr, "Error while parsing\n");
				exit(1);
			}
			//OutputDebugString("%d\n", pkt->size);

			data += ret;
			data_size -= ret;

			if (pkt->size)
				decode(c, frame, pkt, outfilename.c_str());
		}
	}

	/* flush the decoder */
	decode(c, frame, NULL, outfilename.c_str());

	fclose(f);

	av_parser_close(parser);
	avcodec_free_context(&c);
	av_frame_free(&frame);
	av_packet_free(&pkt);

	//writer->release();

	return 0;
}