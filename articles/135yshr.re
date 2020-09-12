= 見られたくないファイルは、見えるところに置いておくと安全かもしれない

== はじめに
人に見られたくないファイルってありますよね。今回はそんなファイルを仮にデスクトップに置いておいてもばれないようにできるかもしれないプログラムの作り方をを紹介します。

=== この章のゴール
1. とりあえず、GitHubからコードをダウンロードして実行できる
2. コードを読んで動きがわかるようなる
3. 改造できるようになる

=== 実行環境
* macOS 10.15.6
* Go 1.14.6+

== 概要
ステガノグラフィーと呼ばれる仕組みを使って、見られたくないファイルを画像ファイルに隠すプログラムを作成します。
記事を読んでステガノグラフィーに興味がでた人は、ネットで検索するといろいろ出てくるので参考にしてみてください。

=== 歴史
ステガノグラフィーの歴史は大変長く紀元前からあるそうです。
よく文献にでてくる話しは、ヘロドトスが戦争中に使ったという話しです。
1. 囚人の頭に重要なメッセージを刺青にして入れる
2. 髪が伸びて入れ墨が見えなくなるまで待つ
3. 囚人を敵陣に送り込む
4. 味方にあったら、髪を剃って頭の入れ墨を味方に見せてメッセージを伝える

有名な脱獄ドラマでも自分の体に刑務所の設計図を刺青にして入れておき、それを隠すように別の刺青をいれていたなんてエピソードもありました。

=== 手法
今回取り上げるステガノグラフィーは、画像に重要な情報を変換して隠しておき、必要になったら隠したデータから情報を取り出すというものです。
コンピュータで扱う画像には、JPEGやGIFやBMPなどいろいろな画像形式がありますが、今回はPNGを対象とします。
また、手法には、LSB法以外にもHCS法や変換領域利用型ステガノグラフィなど多々あるのですが、わかりやすく実装も簡単なLSB法を使っていきます。

LSB法は、Least Significant Bit を略したもので画像データのビットを隠したいデータで置き換える方法です。置き換えるビット数が多いほど、一枚の画像に隠すことはできるデータ量は増えるのですが、画像の見た目が変わってしまうので、見た人は違和感を感じ何か隠していることがバレてしまいます。使用する画像形式や隠すデータの形式を考慮して下位2ビットを置き換えて重要なデータを隠します。

=== PNGフォーマット
プログラムを作成するにあたり、実装前にPNGフォーマットについても簡単に説明します。

PNGは、Portable Network Graphics の略で、"ピング"や"ピン"と呼ばれており、GIFに変わる画像フォーマットとして開発されました。
PNGフォーマットは、PNGシグネチャとチャンクの塊でできています。チャンクには、いろいろな種類があるのですが、最低限定義しなければいけないチャンクは、IHDRチャンク、IDATチャンク、IENDチャンクの３つになります。

PNGシグネチャは、対象のファイルがPNGファイルであることを示すための８バイトデータです。
IHDRチャンクは、ヘッダ情報を配置するする領域で画像の幅や画像の高さ圧縮形式などPNG画像に関する情報が保存されます。
IDATチャンクは、圧縮されたイメージデータ
IENDチャンクは、PNGファイル終端


=== データの隠し方
1. 画像に保存する重要なデータは、1byteずつ取り扱う
2. イメージデータは、1ドットをRGBAとして取り扱う
3. 1.のデータを2ビットずつ4つに分割する
4. R（赤）,G（緑）,B（青）,A（不透明度）それぞれの下位2ビットを分割した文字の2ビットデータで置き換える

== 実行手順
=== インストール
//cmd{
$ go get github.com/135yshr/hide-a-leaf
$ go install github.com/135yshr/hide-a-leaf
//}

=== 使い方
==== ファイルを隠す
元になるファイルと隠したいファイルを指定することで、encode.png が作られます。

//cmd{
$ hide-a-leaf encode {元になるPNGファイル} {隠したいファイル}
//}

==== 隠したファイルを取り出す
隠されたファイルが入っているPNGフィアルを指定することで、decode.png が作られます

//cmd{
$ hide-a-leaf decode {データが隠されたPNGファイル}
//}

=== コードの解説
==== データを画像に隠すコード
//list[hide_data_in_image][データを画像ファイルに隠す][go]{
func encode(cover image.Image, text []byte) (image.Image, error) {
	rect := cover.Bounds()
	newImg := image.NewNRGBA(image.Rectangle{rect.Min, rect.Max})
	index := 0
	for y := rect.Min.Y; y < rect.Max.Y; y++ {
		for x := rect.Min.X; x < rect.Max.X; x++ {
			c1 := color.NRGBAModel.Convert(cover.At(x, y))
			baseColor, ok := c1.(color.NRGBA)
			if !ok {
				continue
			}
			newImg.SetNRGBA(x, y, hiding(baseColor, text, index))
			index++
		}
	}
	return newImg, nil
}

func hiding(c color.NRGBA, text []byte, n int) color.NRGBA {
	var r, g, b, a uint8
	if len(text) <= n || text[n] == 0 {
		r = c.R & 0xfc
		g = c.G & 0xfc
		b = c.B & 0xfc
		a = c.A & 0xfc
	} else {
		r = c.R&0xfc + text[n]&3
		g = c.G&0xfc + (text[n]>>2)&0x3
		b = c.B&0xfc + (text[n]>>4)&0x3
		a = c.A&0xfc + (text[n]>>6)&0x3
	}

	return color.NRGBA{r, g, b, a}
}
//}

==== 画像からデータを復元するコード
//list[restore_hidden_data_from_images][画像に隠したデータを復元する][go]{
func decode(cover image.Image) []byte {
	bs := []byte{}
	rect := cover.Bounds()
	for y := rect.Min.Y; y < rect.Max.Y; y++ {
		for x := rect.Min.X; x < rect.Max.X; x++ {
			i := color.NRGBAModel.Convert(cover.At(x, y))
			c, ok := i.(color.NRGBA)
			if !ok {
				continue
			}
			r := c.R & 0x3
			g := c.G & 0x3 << 2
			b := c.B & 0x3 << 4
			a := c.A & 0x3 << 6
			if (r + g + b + a) == 0 {
				continue
			}
			bs = append(bs, byte(r+g+b+a))
		}
	}
	return bs
}
//}

=== 機能拡張案
TBD

=== さいごに
ステガノグラフィーを使った人に見られたくないファイルの隠し方を紹介しました。
今回のプログラムでは、画像ファイルよりも小さなサイズのファイルしか隠すことができません。
もっと大きなデータを隠せるようにしたいとか、png 以外のファイルにも対応したいというかは、GitHubのリポジトリをForkしてどんどん機能拡張して楽しんでいただけるうれしいです。
ステガノグラフィーは万能ではありません。ステガノグラフィーを解析する手法もありますので、過信しすぎて本当に大切なデータを隠したままインターネットに公開するようなことはないようにしてください。

最後になりますが、稚拙な文章にも関わらず章末まで読んでいただき、本当にありがとうございました。

