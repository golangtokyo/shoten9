= 見られたくないファイルは、見えるところに置いておくと安全かもしれない

== はじめに
人に見られたくないファイルってありますよね。今回はそんなファイルを仮にデスクトップに置いておいてもばれないようにできるかもしれないプログラムの作り方を紹介します。

=== 本章のゴール
 1. とりあえず、GitHubからコードをダウンロードして実行できる
 2. コードを読んで動きが分かるようなる
 3. 改造できるようになる

=== 実行環境
 * macOS 10.15.6
 * Go 1.14.6+

== 概要
ステガノグラフィーと呼ばれる仕組みを使って、見られたくないファイルを画像ファイルに隠すプログラムを作成します。
記事を読んでステガノグラフィーに興味がでた人は、ネットで検索するとステガノグラフィーに関する論文やプログラムが出てきますので、参考にするとより深い知識を得られると思います。

=== 歴史
ステガノグラフィーの歴史はたいへん長く紀元前からあるそうです。
よく文献にでてくる話は、ヘロドトスが戦争中に使ったという話です。

 1. 囚人の頭に重要なメッセージを刺青にして入れる
 2. 髪が伸びて入れ墨が見えなくなるまで待つ
 3. 囚人を敵陣に送り込む
 4. 味方にあったら、髪を剃って頭の入れ墨を味方に見せてメッセージを伝える

有名な脱獄ドラマでも自分の体に刑務所の設計図を刺青にして入れておき、それを隠すように別の刺青をいれていたなんてエピソードもありましたが、あれもステガノグラフィーの利用方法のひとつです。

=== 手法
今回取り上げるステガノグラフィーは、画像に重要な情報を変換して隠しておき、必要になったら隠したデータから情報を取り出すというものです。
コンピュータで扱う画像には、JPEGやGIFやBMPなどいろいろな画像形式がありますが、今回はPNGを対象とします。
また、手法には、LSB法以外にもHCS(Least Significant Bit)法や変換領域利用型ステガノグラフィなど多々あるのですが、わかりやすく実装も簡単なLSB法を使っていきます。

LSB法は、Least Significant Bitを略したもので画像データのビットを隠したいデータで置き換える方法です。置き換えるビット数が多いほど、一枚の画像に隠すことはできるデータ量は増えます。しかし、画像の見た目が変わってしまうので、見た人は違和感を感じ何か隠していることがバレてしまいます。使用する画像形式や隠すデータの形式を考慮して下位2ビットを置き換えて重要なデータを隠します。

=== PNGフォーマット
プログラムを作成するにあたり、実装前にPNGフォーマットについても簡単に説明します。

PNGは、Portable Network Graphics の略で、"ピング"や"ピン"と呼ばれており、GIFに変わる画像フォーマットとして開発されました。
PNGフォーマットは、PNGシグネチャとチャンクの塊でできています。チャンクには、いろいろな種類があるのですが、最低限定義しなければいけないチャンクは、IHDRチャンク、IDATチャンク、IENDチャンクの３つになります。

 * PNGシグネチャは、対象のファイルがPNGファイルであることを示すための８バイトデータ
 * IHDRチャンクは、ヘッダ情報を配置する領域で画像の幅や画像の高さ圧縮形式などPNG画像に関する情報
 * IDATチャンクは、圧縮されたイメージデータ
 * IENDチャンクは、PNGファイル終端


=== データの隠し方
 1. 画像に保存する重要なデータは、1byteずつ取り扱う
 2. イメージデータは、1ドットをRGBAとして取り扱う
 3. 1.のデータを2ビットずつ4つに分割する
 4. R（赤）,G（緑）,B（青）,A（不透明度）それぞれの下位2ビットを分割した文字の2ビットデータで置き換える

== 実行手順
プログラムのインストール方法やインストールしたプログラムの使い方を解説します。

=== インストール
インストールするには、Goの実行環境がインストールされている必要があります。事前に公式サイト@<fn>{golang_org_link}を参考にインストールしておいてください。
//footnote[golang_org_link][@<href>{https://golang.org/}]

//cmd{
$ go get github.com/135yshr/hide-a-leaf
$ go install github.com/135yshr/hide-a-leaf
//}

=== 使い方
ひと通りプログラムの概要を説明したところで、プログラムの使い方を説明します。
プログラムは、指定したPNG画像に隠したいデータを隠す@<code>{encode}機能と画像から隠されたデータを取得する@<code>{decode}機能の２つの機能を提供します。

==== ファイルを隠す
元になるPNGファイルと隠したいファイルをパラメータに指定することで、encode.pngが作られます。

//cmd{
$ hide-a-leaf encode {元になるPNGファイル} {隠したいファイル}
//}

文字列を隠したいときは、@<code>{-text}パラメータを指定すると2つ目パラメータを文字列して取り扱います。
//cmd{
$ hide-a-leaf -text encode {元になるPNGファイル} {隠したい文字列}
//}

==== 隠したファイルを取り出す
見せたくないデータが隠されているPNGファイルを指定してデコードを実行すると、decode.pngが作られます。

//cmd{
$ hide-a-leaf decode {データが隠されたPNGファイル}
//}

文字列として取得したいときは、@<code>{-text}パラメータを指定します。
//cmd{
$ hide-a-leaf -text decode {データが隠されたPNGファイル}
//}

=== コードの解説
実際に使われているコードの中から見せたくないデータを画像に隠すコードと見せたくないデータが隠れたPNGデータから隠したデータを取得する方法を説明します。

==== データを画像に隠すコード

@<list>{hide_data_in_image}に示す@<code>{Encode}関数を使って画像ファイルにデータを隠します。
前提条件として、画像データはデコード済みであり、隠したいデータは@<code>{[]byte}に変換済みである必要があります。

 1. 画像は、左上から読み始め、右に１ドットずつ読み進める
 2. 右端まで読み終わると、読み込むドットの位置をひとつ下げ左端から読み始める
 3. 読み込んだ1ドットのデータは、RGBAに変換し、変換できないときはそのドットを読み飛ばす
 4. 1ドットのデータと隠したいデータ1byteを hiding (ビット置き換え関数）に渡し、その結果を新しい画像データ１ドットとして書き加える
 5. 後は、すべてのドットを読み終わるまで2〜4を繰り返す

//list[hide_data_in_image][データを画像ファイルに隠す][go]{
func Encode(cover image.Image, text []byte) image.Image {
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
	return newImg
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

画像を1ドットずつ読み込みする処理はデータを画像に隠すコードと同じです。本章では、違いがある箇所だけ説明します。

 1. 1ドットのデータのRGBAを１つずつ読み込み、下位2bitを取得する
 2. A,B,G,Rの順に復元する文字の上位ビットとして扱い復元する
 3. 復元した結果、値が0だったときは、復元は完了したと判断し、処理を終了します

//list[restore_hidden_data_from_images][画像に隠したデータを復元する][go]{
func Decode(cover image.Image) []byte {
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
				return bs
			}
			bs = append(bs, byte(r+g+b+a))
		}
	}
	return bs
}
//}


=== さいごに
ステガノグラフィーを使った人に見られたくないファイルの隠し方を紹介しました。
今回のプログラムでは、画像ファイルよりも小さなサイズのファイルしか隠すことができません。
もっと大きなデータを隠せるようにしたいとか、PNG以外のファイルにも対応したいという方は、GitHubのリポジトリをForkしてどんどん機能拡張して楽しんでいただけると、うれしいです。
ただし、ステガノグラフィーは万能ではありません。ステガノグラフィーを解析する手法もありますので、過信しすぎて本当に大切なデータを隠したままインターネットに公開するようなことはないようにしてください。

最後になりますが、読んでいただき本当にありがとうございました。

=== 参考文献
 * 新見 道治 (2003) ディジタルステガノグラフィに関する研究 @<href>{https://core.ac.uk/download/pdf/147423194.pdf}
 * ステガノグラフィで画像に文章を隠蔽してみた（Python） @<href>{https://muroiwataru.net/entry/steganography}
 * Goでステガノグラフィやってみる @<href>{https://blog.applibot.co.jp/2020/03/23/go-steganography/}
