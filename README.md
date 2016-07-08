glfw3_app
=========

GLFW3 application (player)

## glfw_app の概要

・glfw3 ライブラリーを使って動作するアプリケーションと、OpenGL ベースの GUI フレームワークとユーティリティー類です。  
・img_io は画像ファイルの入出力や、画像の操作を網羅するモジュールです。  
・snd_io は音楽ファイルの入出力や、OpenAL を使った音の再生を行うモジュールです。  
・各種ユーティリティーは、文字列の変換、二次元、三次元、四次元ベクトル、行列を扱う数学クラスなどがあります。  

###開発環境（OS-X）

・macports をインストール、各種ライブラリーをインストールして下さい。  

###開発環境（Windows）

・MSYS2、mingw64 環境で開発を行います。  
・pacman で必要なパッケージをインストールして下さい。  
・pacman -S 「パッケージ名」

###必要なライブラリー（パッケージ名）

・mingw-w64-x86_64-clang / clang
・mingw-w64-x86_64-glfw / glfw  　　
・mingw-w64-x86_64-openal / openal  
・mingw-w64-x86_64-freetype / freetype2  
・mingw-w64-x86_64-glew / glew  
・mingw-w64-x86_64-libjpeg-turbo / libjpeg-turbo  
・mingw-w64-x86_64-openjpeg2 / openjpeg  
・mingw-w64-x86_64-libpng / libpng  
・mingw-w64-x86_64-faad2 / faad2  
・mingw-w64-x86_64-libmad / libmad  
・mingw-w64-x86_64-taglib / taglib  
・mingw-w64-x86_64-ffmpeg / ffmpeg  

###各ディレクトリーの構成

・common  --->  各フレームワーク、ラッパー

####便利ツール

・dllcollect  --->  DLL 収集プログラム
・bmc  --->  ビットマップ変換
・iod_make  --->  組み込みマイコン向け、I/O デバイスクラステンプレート生成サポート

####アプリケーション

・gui_test  --->  GUI 描画、操作、テスト用
・player  --->  音楽再生プレイヤー
・image  --->  画像ビューアー
・vplayer  --->  動画再生プレイヤー
・spinv  --->  スペースインベーダーエミュレーター（ ROM イメージ別途要）
・pmdv  --->  MMD プレイヤー
・effv  --->  effekseer プレイヤー

