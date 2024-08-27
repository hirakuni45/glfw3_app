グラフィックス、フレームワーク
=========

[英語版](./README.txt)

## 概要

- 2D の基本的な描画
- 3D の基本的な描画
- 画像ファイルローダー
- シンプルなファイル選択
- シンプルなダイアログ
- 簡単なメニュー機能
- ビットマップフォント
- カラー定義、管理
- グラフィックス関係の操作

---

## ファイル・リスト

|ファイル名|機能|
|---|---|
|[afont.hpp](./afont.hpp)|ASCII フォントクラス|
|[font6x12.hpp](./font6x12.hpp)|6x12 ASCII フォントクラス|
|[font6x12.cpp](./font6x12.cpp)|6x12 ASCII フォントリソース|
|[font8x16.hpp](./font8x16.hpp)|8x16 ASCII フォントクラス|
|[font8x16.cpp](./font8x16.cpp)|8x16 ASCII フォントリソース|
|[kfont.hpp](./kfont.hpp)|漢字フォントクラス|
|[kfont16.cpp](./kfont16.cpp)|16x16 漢字フォントリソース|
|[kfont16.bin](./kfont16.bin)|16x16 漢字フォントバイナリー（240キロバイト）|
|[font.hpp](./font.hpp)|フォント|
|[color.hpp](./color.hpp)|カラー定義|
|[graphics.hpp](./graphics.hpp)|2D 描画クラス|
|[monograph.hpp](./monograph.hpp)|2D ビットマップ描画クラス|
|[simple_filer.hpp](./simple_filer.hpp)|シンプル・ファイル選択クラス|
|[simple_dialog.hpp](./simple_dialog.hpp)|シンプル・ダイアログ（モーダルフレーム）|
|[root_menu.hpp](./root_menu.hpp)|ルートメニュークラス|
|[img.hpp](./img.hpp)|イメージ定義クラス|
|[img_in.hpp](./img_in.hpp)|画像定義|
|[pixel.hpp](./pixel.hpp)|ピクセル定義|
|[bmp_in.hpp](./bmp_in.hpp)|BMP ファイルローダー|
|[jpeg_in.hpp](./jpeg_in.hpp)|JPEG ファイルローダー|
|[png_in.hpp](./png_in.hpp)|PNG ローダー|
|[picojpeg.h](./picojpeg.h)|picojpeg ヘッダー|
|[picojpeg.c](./picojpeg.c)|picojpeg ソースコード|
|[picojpeg_in.hpp](./picojpeg_in.hpp)|picoJPEG ローダー|
|[scaling.hpp](./scaling.hpp)|スケーリングパイプクラス|
|[glmatrix.hpp](./glmatrix.hpp)|OpenGL マトリックスクラス|
|[tgl.hpp](./tgl.hpp)|TinyGL(Tiny OpenGL) クラス|
|[shape_3d.hpp](./shape_3d.hpp)|3D シェープ（簡単なモデル）|
   
---

## 画像ローダー

- [img_in.hpp](./img_in.hpp){:target="_blank"}
- [scaling.hpp](./scaling.hpp){:target="_blank"}

通常、画像ファイルを扱う場合、一旦メモリにロードして、スケールなど行いフレームバッファに書き込みます。   
しかし、画像ファイルは巨大なメモリを使用するので、内蔵メモリでは、足りない場合もあります。   
この画像ローダーテンプレートでは、テンポラリメモリを使用しないで、直接フレームバッファに描画します。   
   
スケーリングテンプレートクラスを描画ファンクタの間に定義すれば、スケーリングしながら描画を行う事が出来ます。   
構造上、スケーリング品質は、簡易的なものですが、ある程度、エイリアシングを除去する事が出来るように工夫してあります。   
スケーリングでは、縮小時に平均を行う為、描画先のフレームバッファを「黒」でクリアしておく必要があります。   
   
- img::img_in クラスによる BMP, PNG, JPEG 画像のロードと展開（自動判別）
- 「展開」を行うファンクタをテンプレートで指定する構成（省メモリ）
- img::scaling クラスを経由する事で、拡大、縮小が可能

```C++
		typedef img::scaling<RENDER> SCALING;
		SCALING		scaling_;

		typedef img::img_in<SCALING> IMG_IN;
		IMG_IN		img_in_;

		// constructor: scaling_(render_), img_in_(scaling_),

		scaling_.set_scale();
		render_.flush();
		img_in_.load("/NoImage.jpg");
```

---

## 簡易ダイアログ simple_dialog.hpp

- 単独で、ダイアログを表示する事が出来る。
- メッセージを表示し、タッチで消える、簡単なポリシー

```C++

```

---

## TinyGL (Tiny OpenGL)

- OpenGL ライクな API を持った３Ｄグラフィックスライブラリ
- リアルタイム表示を行う為には、ダブルバッファが必要（RX72N、RX72M）
- OpenGL と互換性のある行列演算と API (glmatrix.hpp)

※現在開発中

---
   
License
---

MIT
