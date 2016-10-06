glfw3_app
=========

GLFW application

## glfw_app の概要

・glfw ライブラリーを使って動作するアプリケーションと、OpenGL ベースの GUI フレームワークとユーティリティー類です。  
・img_io は画像ファイルの入出力や、画像の操作を網羅するモジュールです。  
・snd_io は音楽ファイルの入出力や、OpenAL を使った音の再生を行うモジュールです。  
・utils は、文字列の操作、二次元、三次元、四次元ベクトル、行列を扱う数学クラスなどがあります。  

###開発環境（OS-X）

・macports をインストール、各種ライブラリーをインストールして下さい。  

###開発環境（Windows）

・msys2、mingw64 環境で開発を行います。  
・pacman で必要なパッケージをインストールして下さい。  
・pacman -S 「パッケージ名」

###必要なライブラリー（パッケージ名：Windows / OS-X / Ubuntu-64）

・mingw-w64-x86_64-clang / clang / clang   
・mingw-w64-x86_64-boost / boost / libboost-dev   
・mingw-w64-x86_64-glfw / glfw / libglfw3-dev   
・mingw-w64-x86_64-openal / openal / libopenal-dev   
・mingw-w64-x86_64-libpng / libpng / libpng12-dev   
・mingw-w64-x86_64-freetype / freetype / libfreetype6-dev  
・mingw-w64-x86_64-glew / glew / libglew-dev  
・mingw-w64-x86_64-libjpeg-turbo / libjpeg-turbo / libjpeg-turbo8-dev  
・mingw-w64-x86_64-openjpeg2 / openjpeg / libopenjp2-7-dev   
・mingw-w64-x86_64-faad2 / faad2 /    
・mingw-w64-x86_64-libmad / libmad / libmad0-dev   
・mingw-w64-x86_64-taglib / taglib / libtaglib
・mingw-w64-x86_64-ffmpeg / ffmpeg  
・mingw-w64-x86_64-bullet / bullet   

※ ubuntu-ja-16.04-desktop-amd64 の場合、libpng12-dev と libfreetype6-dev の
組み合わせが必要でした。
※ ubuntu-ja-16.04-desktop-amd64 の場合、/usr/include/openjpeg.h のシンボリック
リンクが、openjpeg-1.5 のものなので、openjpeg-2.1 に張りなおしました。
```
   cd /usr/include
   sudo ln -s -f openjpeg-2.1/openjpeg.h openjpeg.h
```


###フォントのインストール

GUI フレームワークでは、「Inconsolata」の OTF フォントを使っています。   
ダウンロードして、インストールして下さい。


   
<a href="http://levien.com/type/myfonts/inconsolata.html" target="_blank">Inconsolata OTF font</a>   
   

###全体のソース・コード取得
```
   git clone git://github.com/hirakuni45/glfw3_app.git
```

###各ディレクトリーの構成

・common  --->  フレームワーク、ラッパー

###コンパイル方法
・各ディレクトリーに移動後、「make」を行なう。   
・従属規則は自動で生成される。   

####便利ツール   

・dllcollect  --->  mingw64 DLL 収集プログラム   
```
   dllcollect 「実行ファイル」 
```
実行ファイルに必要な、DLL を「dlls」ディレクトリーに全てコピーする。   
   
・bmc  --->  ビットマップ変換   
・iod_make  --->  組み込みマイコン向け、I/O デバイスクラステンプレート生成サポート   

####アプリケーション   

・gui_test  --->  GUI 描画、操作、テスト用   
・player  --->  音楽再生プレイヤー   
![player アプリ](player.png)   
   
・image  --->  画像ビューアー   
![image アプリ](image.png)        
   
・vplayer  --->  動画再生プレイヤー(ffmpeg)   
・spinv  --->  スペースインベーダーエミュレーター（ROM イメージ別途必要）   
![spinv アプリ](spinv.png)   
   
・pmdv  --->  MMD プレイヤー 
![pmdv アプリ](pmdv.png)     
   
・effv  --->  effekseer プレイヤー   

---
License

MIT
