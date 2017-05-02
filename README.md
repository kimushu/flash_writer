# peridot\_flash\_writer

このリポジトリは、NiosII HALシステム上に、フラッシュデバイスの読み書き機能を有する疑似ファイル(ブロックデバイス)を提供するパッケージです。
以下のフラッシュデバイスに対応しています。

- MAX 10 内蔵フラッシュ(UFM, CFM)
- SPI接続フラッシュROM (altera\_avalon\_spi経由)
- SPI接続フラッシュROM (peridot\_swi経由)

## 対象システム

NiosIIベースソフトウェア環境

## 使い方

- Quartusプロジェクトフォルダ直下の"ip"フォルダ内に、このリポジトリをクローンする。
- NiosII EDS 上で BSP Editor を開き、Software Packages タブの一覧から「peridot\_flash\_writer」にチェックをつける
- 適宜設定を編集したのち、Generate BSP を実行する。

## 設定項目

- peridot\_flash\_writer.root
  - name
    - 疑似ファイルが作成されるパスを指定します。例えば /aaa/bbb を指定すると、疑似ファイル ccc のパスは /aaa/bbb/ccc となります。

- peridot\_flash\_writer.on\_chip\_flash
  - enabled
    - MAX 10 内蔵フラッシュの疑似ファイルを生成する場合にチェックします。
  - name
    - altera\_onchip\_flash IPが登録するデバイスのパスを指定します。IPの名前を xxx とすると、通常は /dev/xxx\_data です。

- peridot\_flash\_writer.spi
  - TODO
  
## 使用例

- peridot\_flash\_writer.root.nameがデフォルト値"/sys/flash"であるとき、MAX10のUFM0にデータを書き込む例
  ```c:prog_ufm0.c
  unsigned char data[] = {...};
  int fd = open("/sys/flash/ufm0", O_RDWR);
  write(fd, data, sizeof(data));
  close(fd);
  ```

## MAX 10 内蔵フラッシュの扱い

MAX 10 内蔵フラッシュは、複数のUFMおよびCFMからなる5つのセクタで構成されます。
本パッケージでは、以下の疑似ファイルが生成されます。

<table>
<tr><th>セクタ番号</th><th>1</th><th>2</th><th>3</th><th>4</th><th>5</th></tr>
<tr><td rowspan="2">疑似ファイル名</td><td align="center">ufm1</td><td align="center">ufm0</td><td align="center">cfm2</td><td align="center">cfm1</td><td align="center">cfm0</td></tr>
<tr><td colspan="2" align="center">ufm (*)</td><td colspan="2" align="center">image1</td><td align="center">image0</td></tr>
</table>

(*)…Single configurationの場合はセクタ3以降もUFMに含まれるケースがありますが、本パッケージでのufmは常にセクタ1～2のみを指します。

なお、各セクタの役割やサイズについての詳細は、メーカーの[UFM資料](https://www.altera.com/en_US/pdfs/literature/hb/max-10/ug_m10_ufm.pdf)および[コンフィグレーション資料](https://www.altera.com/en_US/pdfs/literature/hb/max-10/ug_m10_config.pdf)を参照してください。
imageの名称はメーカー公式資料内でも統一されておらず混乱が見られるため、本IPではコンフィグレーション資料の呼称(image0/1)に統一しています。
