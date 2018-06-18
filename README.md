# syspro2018-baremetal
このリポジトリは2018年度システムプログラミング実験ベアメタルプログラミング演習資料です。

## lecture\*
各演習回用のソースコード（及びビルド環境）です。
詳細は演習資料を参照してください。

## pure_baremetal
このディレクトリ内で```make```する事で、完全なベアメタル環境で動かすためのディスクイメージを作成できます。
BIOS, UEFI環境の双方で起動でき、ブートローダーとしてGRUB2が立ち上がるようになっています。

### disk.img.tar.xz
ブートファームウェアから起動するためにはディスクを適切に初期化する必要があります(e.g. MBRの書き込み等)が、これを最初から提供するのがdisk.imgです。


このdisk.imgを生成するコードは以下のリポジトリにあります。
https://github.com/liva/baremetal_diskimage


disk.img.tar.xzはこのdisk.imgを圧縮した物です。

## LICENSE
本リポジトリ内の全てのファイルはMITライセンスのもとで公開されています。詳細はLICENSE.txtをご覧ください。