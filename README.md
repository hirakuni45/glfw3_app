glfw3_app
=========

GLFW application

## glfw_app の概要

・glfw ライブラリーを使って動作するアプリケーションと、OpenGL ベースの GUI フレームワークとユーティリティー類です。  
・img_io 画像ファイルの入出力や、画像の操作を網羅するモジュールです。  
・snd_io 音楽ファイルの入出力や、OpenAL を使った音の再生を行うモジュールです。  
・utils 文字列の操作、二次元、三次元、四次元ベクトル、行列を扱う数学クラスなどがあります。  
・gl_fw OpenGL API 関係の C++ ラッパーなど。
・core glfw API 関係の C++ ラッパー、フォントの描画関連など。
・widgets GUI フレームワークなど
・av ffmpeg API 関係の C++ ラッパー
   
### 開発環境（Windows）
   
 - Windows では、事前に MSYS2 環境をインストールしておきます。
 - MSYS2 には、msys2、mingw32、mingw64 と３つの異なった環境がありますが、mingw64 で行います。 
   
 - msys2 のアップグレード

```sh
   update-core
```

 - コンソールを開きなおす。（コンソールを開きなおすように、メッセージが表示されるはずです）

```sh
   pacman -Su
```
 - アップデートは、複数回行われ、その際、コンソールの指示に従う事。
 - ※複数回、コンソールを開きなおす必要がある。

```
   pacman -S gcc
   pacman -S make
   pacman -S git
   pacman -S mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-clang
   pacman -S mingw-w64-x86_64-boost   
   pacman -S mingw-w64-x86_64-glfw
   pacman -S mingw-w64-x86_64-openal
   pacman -S mingw-w64-x86_64-freetype
   pacman -S mingw-w64-x86_64-glew
   pacman -S mingw-w64-x86_64-libjpeg-turbo
   pacman -S mingw-w64-x86_64-openjpeg2
   pacman -S mingw-w64-x86_64-libpng
   pacman -S mingw-w64-x86_64-faad2
   pacman -S mingw-w64-x86_64-libmad
   pacman -S mingw-w64-x86_64-taglib
   pacman -S mingw-w64-x86_64-ffmpeg
   pacman -S mingw-w64-x86_64-bullet
```
   
### 開発環境（OS-X）

・macports をインストール、各種ライブラリーをインストールして下さい。  
   
```
   sudo port install clang
   sudo port install boost
   sudo port install glfw
   sudo port install openal
   sudo port install freetype
   sudo port install glew
   sudo port install libjpeg-turbo
   sudo port install openjpeg
   sudo port install libpng
   sudo port install faad2
   sudo port install libmad
   sudo port install taglib
   sudo port install ffmpeg
   sudo port install bullet
```
   
### 開発環境（Ubuntu）

Linux 環境は、複数あるので、ここでは「Ubuntu 16.04 LTS」環境の場合を書いておきます。
```
   sudo apt-get install xxx
```
   
### フォントのインストール

GUI フレームワークでは、「Inconsolata」の OTF フォントを使っています。   
ダウンロードして、インストールして下さい。
   
<a href="http://levien.com/type/myfonts/inconsolata.html" target="_blank">Inconsolata OTF font</a>   
   
Linux 環境では、apt−get 等でインストールできます。
```
   sudo apt-get install fonts-inconsolata
```
   
### 全体のソース・コード取得
```
   git clone git://github.com/hirakuni45/glfw3_app.git
```

### コンパイル方法
・各ディレクトリーに移動後、「make」を行なう。   
・従属規則は自動で生成される。   

#### 便利ツール   

・dllcollect  --->  mingw64 DLL 収集プログラム   
```
   dllcollect 「実行ファイル」 
```
実行ファイルに必要な、DLL を「dlls」ディレクトリーに全てコピーする。   
   
・bmc  --->  ビットマップ変換   
・iod_make  --->  組み込みマイコン向け、I/O デバイスクラステンプレート生成サポート   

#### アプリケーション   

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
