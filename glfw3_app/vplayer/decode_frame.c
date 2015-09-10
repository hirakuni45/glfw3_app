#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

int main(int argc, char **argv)
{
	// register all the codecs
	avcodec_register_all();
	av_register_all();
	avfilter_register_all();

	if(argc < 2) {
		return 0;
	}

	// ビデオを開く
	AVFormatContext *pFormatCtx = NULL;
	if(avformat_open_input(&pFormatCtx, argv[1], NULL, NULL) != 0) {
		printf("ERROR: avformat_open_input() failed. (%s, %d)\n", __FILE__, __LINE__);
		return -1;
	}
 
	// ストリーム情報の取得
	if(avformat_find_stream_info(pFormatCtx, NULL) < 0) return -1;
	av_dump_format(pFormatCtx, 0, argv[1], 0);

	// コーデックの検索
	AVCodecContext *pCodecCtx = pFormatCtx->streams[0]->codec;
	AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

	// コーデックを開く
	if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		printf("ERROR: avcodec_open2() failed. (%s, %d)\n", __FILE__, __LINE__);
		return -1;
	}

	// log level の設定
	av_log_set_level(1);

	// フレームの確保
	AVFrame *pFrame = avcodec_alloc_frame();
	AVFrame *pImage = avcodec_alloc_frame();

	// バッファの確保
	unsigned char *buffer = (unsigned char *)av_malloc(avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height));
	// バッファとフレームを関連付ける
	avpicture_fill((AVPicture*)pImage, buffer, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);

	struct SwsContext *pSWSCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height
			, pCodecCtx->pix_fmt
			, pCodecCtx->width, pCodecCtx->height
			, PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
//			, PIX_FMT_RGB24, SWS_SPLINE, NULL, NULL, NULL);

	// メインループ
	int count = 0;
	AVPacket packet;

	// フレーム読み込み
	while(av_read_frame(pFormatCtx, &packet) >= 0) {

		// デコード
		int state;
		avcodec_decode_video2(pCodecCtx, pFrame, &state, &packet);
 
		// デコード完了
		if(state) {
			int bsize = pCodecCtx->width * pCodecCtx->height * 3;
//			avpicture_layout(pFrame, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, buffer, bsize);
			sws_scale(pSWSCtx, (const uint8_t **)pFrame->data
			, pFrame->linesize, 0 , pCodecCtx->height
			, pImage->data, pImage->linesize);

			// 特定のフレームを切り出し
			if(count == 400) {
				FILE *fp = fopen("rgb24.raw", "wb");
				if(fp) {
					printf("%d, %d\n", pCodecCtx->width, pCodecCtx->height);
					fwrite(buffer, bsize, 1, fp);
					fclose(fp);
				}
			}
			++count;
		}
 
		// メモリ解放
		av_free_packet(&packet);
	}
	printf("Total frame: %d\n", count);

	sws_freeContext(pSWSCtx);
	av_free(buffer);
	av_free(pImage);
	av_free(pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}
