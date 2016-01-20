glfw3_app
=========

GLFW3 application (player)

## glfw_app の概要

・glfw3 ライブラリーを使って動作するアプリケーションと、OpenGL ベースの GUI フレームワークとユーティリティー類です。  
・img_io は画像ファイルの入出力や、画像の操作を網羅するモジュールです。  
・snd_io は音楽ファイルの入出力や、OpenAL を使った音の再生を行うモジュールです。  
・glfonts は freetype2 を使ったフォントの描画を行い、フォント毎にテクスチャーを割り当て管理して表示します。  
・glmobj は、大きさに依存しない画像管理を行います。  
・各種ユーティリティーは、文字列の変換、二次元、三次元、四次元ベクトル、行列を扱う数学クラスなどがあります。

###開発環境（APPLE）
・macports をインストール、各種ライブラリーをインストールして下さい。

###開発環境（Windows）

・MSYS2、mingw64 環境で開発出来ます。
・pacman で必要なパッケージをインストールして下さい。

###必要なライブラリー
・glfw3
・glew
・libpng
・toubojpeg
・openjpeg
・freetype2
・libmad
・faad2
・taglib
・openal
・ffmpeg
