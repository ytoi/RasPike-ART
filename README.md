# RasPike-ART (Almost Real Time)

RasPike-ARTはLEGO® Education SPIKE™ PrimeとRaspberryPiをUSBで接続して、Raspberry上で[SPIKE-RT](https://github.com/spike-rt/spike-rt)のAPIを使ってSPIKEを制御することのできるライブラリです。
すでに公開されている[RasPike](https://github.com/ETrobocon/RasPike)の後継ですが、以下の点で異なっています。

- SPIKE側のプログラムがmicro pythonではなく、SPIKE-RTを使用しており、リアルタイム性が高まっている
　RasPikeのデータ送信周期は30msecほどかかっていましたが、RasPike-ARTでは10msecとなっています（もう少し早くなる可能性あり)
- USB接続を用いているので、ポートがA-F全て利用可能
- RasPikeはETロボコン用として決まった構成（ポートと繋ぐもの）に限定されていましたが、任意のデバイスをつなげて利用することができます
- EV3RTのAPIではなく、SPIKE-RTのAPIを使うことができるため、SPIKEから得られる情報量が多いです
　ジャイロは３軸の情報が取れますし、Hubのディスプレイ、スピーカーなども利用できます
- EV3RT（ASP3)とは独立しているので、通常のLinuxプログラムで使用することができます


# 構成

RaspberryPiのUSB（ホスト）とSPIKEのUSB(デバイス）をUSBケーブルで繋ぎます。
あとは任意の構成で使用できます。
RasPikeで使用していたシリアルケーブルは不要です。

# 環境


*現在、動作確認はDebian version: 11 (Bullseye)32bit Legacyでのみ行っています。そのほかでも動くかもしれませんが、まだ試していません。RasPikeが動作した環境であればそのまま使えます。*


## 最初の設定

RasberryPi上の任意の場所でGitHubからcloneします。
   
```
git clone https://github.com/ytoi/RasPike-ART.git
```

spikeディレクトリに移動します。

```
cd RasPike-ART/spike
```

必要なモジュールのインストールを行うため、セットアップのスクリプトを呼び出します。

```
spike_env_setup_linux.sh
```

ちなみに、spike_env_setup_mac.shもありますが、macでの動作はまだできていません。
RasPike-ARTの下にtooolsができているはずです。
このセットアップはSPIKEにプログラムを送るためのものです。[SPIKE-RTの環境構築](https://github.com/spike-rt/spike-rt/blob/main/docs/ja/Env.md)のSPIKEへの書き込みのところを参考にしています。

## SPIKEへの受け側プログラムの書き込み

この作業は一度だけやれば良いです。ただし、SPIKE側のプログラムが更新された場合には再度焼き直しが必要になります。

RaspberryPiにUSBケーブルを繋ぎます。SPIKEは電源を切っておいてください。SPIKEのBluetooth接続ボタン（本体の右上の丸いボタン)を押しながら、USBを接続します。ボタンの色が変化するまで押し続けてください。
ボタンがカラフルに変化したらボタンから手を離してください。これでDFUモードに入ります。

その後、以下のコマンドを打ちます。
```
spike_update_linux.sh
```

メッセージが流れますが、最後に
```
Writing memory...
0x08008000  130600 [=========================] 100% 
Exiting DFU...
Finished
+ rm -rf firmware.dfu
```
という表示が出て、SPIKEに「ET」の文字が出れば焼き込み終了です。

## RaspberryPi側のプログラムの実行

sampleディレクトリで作業を行います。まず、あらかじめ格納してあるraspike_testを動かしてみます。
コードはsample/rapike_test/raspike_test.c にあります。
デフォルトではカラーセンサーのテストをするので、Fポートにカラーセンサーをつないでおいてください。

RasPike-ARTディレクトリの直下にあるbuild/obj-raspike_testに移動してmakeします。

```
cd build/obj-raspike_test
make
```
これでraspike_testができるはずです。

実行前にSPIKE側の電源を一度切ります（センターボタンを長押しして、3x3の四角の表示がされるのを確認してください）。
再度センターボタンを押して、「ET」表示になるのを確認してください。
そうしたら、raspike_testを実行します。

```
./raspike_test
```

これで以下のようにカラーセンサーの値が取れれば成功です。

```
[RGB]r=18 g=21 b=21
[RGB]r=17 g=20 b=20
[RGB]r=18 g=20 b=20
[RGB]r=17 g=20 b=20
[RGB]r=3 g=4 b=3
[RGB]r=7 g=8 b=8
[RGB]r=7 g=8 b=8
[RGB]r=6 g=8 b=8
[RGB]r=6 g=8 b=8
[RGB]r=6 g=8 b=7
```

# SPIKEのAPI対応
SPIKE-RTのAPIは以下を参照してください。
[SPIKE-RT APIのページ](https://spike-rt.github.io/spike-rt/en/html/index.html)

RasPike-ARTでは上記のAPIは一部を除いて対応しています。

非対応API
- pup_color_sensor_detectable_colors(そのうち対応予定)
- Bluetooth API

対応しているが、呼ぶとSPIKE-RT側で死んでしまうもの（呼ばないよう注意)
- pup_ultrasonic_sensor_light_set
- pup_ultrasonic_sensor_light_on
- pup_ultrasonic_sensor_light_off

また、注意事項として、モーターについては
　pup_motor_get_device()でポートに関連づけたあと、pup_motor_setup()をしてからpup_motor_set_power()などを呼ぶようにしないと、SPIKE-RT側で死んでしまうようです。

この辺りのエラーチェックはそのうち改善します。

## 電源の落とし方
先にRasberryPi側のコンソールでctrl+cを押し、プログラムを中断して下さい。その後、SPIKEのセンターボタンを長押しして電源を切って下さい。
この手順でやらないとUSBポートが変わったり、使えなくなったりします。


# オリジナルのアプリを作る

## ディレクトリの追加
コードはsampleの下にディレクトリを作って、そこにファイルをおいてください。このディレクトリ名がアプリ名になります。
buildの下にobj-(アプリ名)のディレクトリを作ります。
obj-raspike_test/Makefileをコピーして、obj-(アプリ名)の下におきます。

## Makefile

Makefileを修正し、以下を修正します。

| 変数名 | 説明 |
|------|------|
|APP_NAME|アプリの名前|
|SRCS  |ソースコード一覧|
|INCLUDES|追加するインクルードパス(-Iをつけること|
|OPT   |最適化オプション(問題なければそのままで良い)|

## RasPike-ART API

RasPIke-ARTのAPIはincludeの下にあります。

### 通信用のAPI

raspike_com.h
```
 RPComDescriptor *raspike_open_usb_communication(const char *device_name);
```

USBの通信を開通させます。device_nameにはSPIKEをUSB接続した時の名前を指定します。
通常は/dev/ttyACM0 が使用されており、これを指定すれば良いのですが、他のものを繋げたり、電源の落とし方が悪いとttyACM1になったり、/dev/ttyACMx が見えなくなったりします。

```
ls /dev/ttyACM*
```
で確認しましょう。ない場合は、RasberryPiのUSBポートを他のところにしてみるか、RaspberryPiをリブートしてみて下さい。

### RasPike-ARTのAPI

raspike_protocol_api.h
```
  extern int raspike_prot_init(RPComDescriptor *desc);
  extern int raspike_prot_receive(void);
  extern int raspike_prot_shutdown(void);
```

- raspike_prot_init()
  descにはraspike_open_usb_communication()で取得されたRPComDescriptorのポインタを指定します。
  この中でSPIKEとのハンドシェイクが行われます。

- raspike_prot_receive()
  SPIKEからのメッセージを受け取る関数です。プログラム内でこの関数を別スレッドで永久ループの中で呼び出して下さい。
  raspike_test.cのコードを参考にしてみて下さい。
  この関数はraspike_prot_init()の後に実行されるようにしてください。

- raspike_prot_shutdown()
  SPIKEのシャットダウンを行います。


# 実行時のエラー
　SPIKE側のプログラムで何か異常が起こった場合、２桁の数字をSPIKE上に表示して、センターボタンが赤くなります。この時の数字がわかると不具合の理由がわかるかもしれません。

# SPIKE側のコード
　SPIKEのコードはspike-rtをフォークした場所で作っています。
 https://github.com/ytoi/spike-rt.git
 （まだコミットが不完全なので、cloneしても動作しないと思います)
