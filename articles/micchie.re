= たのしいUML

== はじめに
UMLを書く機会の多い筆者が、「それでもGoで書きたい」を実現させるために調べたパッケージを紹介します。
パッケージの紹介の前に、UMLについて、そしてUMLをするためのツールであるPlantUMLについても触れています。

=== この章のゴール
UMLのクラス図をGoで書くことができるようになります。

=== 検証環境
この章では、次の環境で検証しています。

 * macOS 10.15.6
 * Go 1.15.1

== UMLとは
UML（Unified Modeling Language）@<fn>{UML}は、設計をモデル化するときの記法を統一した言語です。システムの構造や振る舞いを図で表すことができ、次のようなバリエーションがあります。（一部のみ抜粋）
//footnote[UML][@<href>{https://www.uml.org}]

==== 構造図
 * クラス図
 * コンポーネント図
 * オブジェクト図
 * パッケージ図

==== 振る舞い図
 * ユースケース図
 * アクティビティ図
 * 状態遷移図
 * シーケンス図

//image[001][シーケンス図の例][width=0.5\maxwidth]

UMLを書く方法はさまざまです。Lucidchart@<fn>{Lucidchart}、Cacoo@<fn>{Cacoo}、draw.io@<fn>{draw.io}など、オンラインのツールは豊富です。
さらに、テキストでUMLを書くこともできます。テキストで表現できるUMLは変更管理のしやすさを鑑みても大変魅力的です。
テキストでUMLを書くことができるPlantUML@<fn>{PlantUML}について、@<hd>{sectionName}で紹介をします。
//footnote[Lucidchart][@<href>{https://www.lucidchart.com/pages/ja/examples/uml-tool}]
//footnote[Cacoo][@<href>{https://cacoo.com/ja/}]
//footnote[draw.io][@<href>{https://app.diagrams.net}]
//footnote[PlantUML][@<href>{https://plantuml.com}]

=={sectionName} PlantUML
PlantUMLは、テキストをUMLに変換、出力できるツールです。シーケンス図においてはアスキーアートで出力することもできます。
さらにUML以外にも次のような表現にも対応しており、常に機能がアップデートされ続けています。

 * ワイヤーフレーム
 * アーキテクチャ図
 * ガントチャート
 * マインドマップ
 * WBS

@<img>{001}は、次のコードから生成されています。

//emlist{
@startuml
participant "自転車" as Bicycle #lightpink
participant "クロクマ" as BlackBear #lightskyblue

Bicycle -> BlackBear :ぶつかる
BlackBear -> BlackBear: 転ぶ

loop
    activate BlackBear
    BlackBear --> Bicycle: 激怒
    Bicycle -> BlackBear: 謝る
    deactivate BlackBear
end

Bicycle -> Bicycle: 逃げる
@enduml
//}

=== PlantUMLのインストール
PlantUML公式のオンラインサーバを利用せずに、手元のPCでPlantUMLからUMLを出力するには、Javaの実行環境が不可欠です。
まずはJavaの環境を用意しておきましょう。

次に必要なものはグラフデータを描画するためのGraphviz@<fn>{GraphvizDownload}です。
シーケンス図とアクティビティ図以外のUMLを出力するには必要なソフトウェアです。
インストールしておきましょう。
//footnote[GraphvizDownload][@<href>{http://www.graphviz.org/download/}]

最後に、PlantUMLの実行ファイルはダウンロードページ@<fn>{PlantUMLDownload}から入手できます。
//footnote[PlantUMLDownload][@<href>{https://plantuml.com/ja/download}]

macOSの場合は、Homebrewからインストールできます。

//cmd{
$ brew install graphviz
$ brew install plantuml
//}

=== PlantUMLでUMLを生成する
UMLを書いたテキストファイルを指定することで、図（png）が生成されます。ファイルが格納されたディレクトリそのものを指定することもできます。

//cmd{
$ java -jar plantuml.jar {UMLを書いたファイル}
//}

もしくは、Homebrewでインストールした場合は、次のコマンドです。

//cmd{
$ plantuml {UMLを書いたファイル}
//}

なお、PlantUMLの豊富なオプションは@<code>{-h}で確認することができます。

//cmd{
plantuml -h
Usage: java -jar plantuml.jar [options] -gui
	(to execute the GUI)
    or java -jar plantuml.jar [options] [file/dir] [file/dir] [file/dir]
	(to process files or directories)

You can use the following wildcards in files/dirs:
	*	means any characters but '/'
	?	one and only one character but '/'
	**	means any characters (used to recurse through directories)

where options include:
    -gui		To run the graphical user interface
    -tpng		To generate images using PNG format (default)
    -tsvg		To generate images using SVG format
    -teps		To generate images using EPS format
    -tpdf		To generate images using PDF format
    .
    .
    .
//}

UMLをテキストで書けるようになりました。

== PlantUMLをGoで描く
いよいよ、UMLをGoで書きはじめます。

この節では、@<code>{jfeliu007/goplantuml}@<fn>{goplantuml}というツールを利用することで、GoのコードからPlantUMLのクラス図を生成する方法を紹介します。
//footnote[goplantuml][@<href>{https://github.com/jfeliu007/goplantuml/}]

=== jfeliu007/goplantumlのインストール
@<code>{jfeliu007/goplantuml}のGitHubページに記載されているとおりにインストールします。
@<code>{go get}を利用するには、あらかじめGoがインストールされている必要があります。

//cmd{
$ go get github.com/jfeliu007/goplantuml/parser
$ go get github.com/jfeliu007/goplantuml/cmd/goplantuml
$ cd $GOPATH/src/github.com/jfeliu007/goplantuml
$ go install ./...
//}

=== jfeliu007/goplantumlの実行
Goで書いたクラスが格納されたディレクトリを指定して実行することで、PlantUMLのテキストファイルを生成できます。

//cmd{
$ goplantuml path/to/gofiles
//}

指定したディレクトリを再帰的に読み込んですべてのGoファイルを対象とするためには、@<code>{-recursive}オプションを指定します。

//cmd{
$ goplantuml -recursive path/to/gofiles
//}

ファイルに書き出すこともできます。

//cmd{
$ goplantuml -recursive path/to/gofiles > filename.uml
//}

@<code>{jfeliu007/goplantuml}の実行オプションは@<code>{-h}もしくは@<code>{-help}を指定することで確認できます。そのうちのいくつかを紹介します。

 : -recursive
    このオプションを指定しているときは、ディレクトリ内のGoファイルを再帰的に図として出力します。

 : -hide-connections
    図内の接続を非表示にします。

 : -hide-fields
    フィールドを非表示にします。

 : -hide-methods
    メソッドを非表示にします。

 : -ignore string
    描画から除外するディレクトリをカンマ区切りで指定します。

 : -output string
    標準出力を利用していない場合、出力されるファイル名を指定します。

 : -title string
    図のタイトルを引数として指定することができます。

//pagebreak

=== GoでUMLを書く
UMLで出力したい構造をGoで書きます。
この章では、bearというディレクトリを作成し、3つのGoのファイル@<code>{bear.go}、@<code>{blackbear.go}、@<code>{polerbear.go}を作成します。

最初に、@<code>{Bear}という構造体を用意します。

//emlist[bear/bear.go][go]{
package bear

import "fmt"

type BearInterface interface {
	Sleep()
	Eat()
}

type Bear struct {
	Name  string
	Color string
}

func (b *Bear) Sleep() {
	fmt.Printf("%s%sは眠る\n", b.Color, b.Name)
}

func (b *Bear) Eat() {
	fmt.Printf("%s%s食べる\n", b.Color, b.Name)
}
//}

@<code>{BlackBear}には@<code>{Bear}が埋め込まれ、@<code>{Climbing}というメソッドを持っています。

//emlist[bear/blackbear.go][go]{
package bear

import "fmt"

type BlackBear struct {
	Bear
}

func (b *BlackBear) Climbing() {
	fmt.Printf("%s%sは木登りをする\n", b.Color, b.Name)
}
//}

@<code>{PolerBear}にも@<code>{Bear}が埋め込まれ、@<code>{Swim}というメソッドを持っています。

//emlist[bear/polerbear.go][go]{
package bear

import "fmt"

type PolerBear struct {
	Bear
}

func (p *PolerBear) Swim() {
	fmt.Printf("%s%sは海で泳ぐ\n", p.Color, p.Name)
}
//}

ファイル構成は次のとおりです。

//cmd{
├── bear
│   ├── bear.go
│   ├── blackbear.go
│   └── polerbear.go
.
.
.
//}

実際にBearやBlackBear、PolerBearを呼び出し、実行してみましょう。

//emlist[main.go][go]{
func main() {
	b := bear.Bear{}
	b.Name = "Bear"

	b.Sleep()
	b.Eat()

	fmt.Println("-----")

	bb := bear.BlackBear{}
	bb.Name = "熊"
	bb.Color = "黒"

	bb.Sleep()
	bb.Eat()
	bb.Climbing()

	fmt.Println("-----")

	pb := bear.PolerBear{}
	pb.Name = "クマ"
	pb.Color = "シロ"

	pb.Sleep()
	pb.Eat()
	pb.Swim()
}
//}

次のように、出力されます。

//cmd{
Bearは眠る
Bear食べる
-----
黒熊は眠る
黒熊食べる
黒熊は木登りをする
-----
シロクマは眠る
シロクマ食べる
シロクマは海で泳ぐ
//}

次に、@<code>{bear}ディレクトリを指定して@<code>{goplantuml}を実行し、@<code>{bear.uml}というファイルにUMLを書き出しましょう。

//cmd{
$ goplantuml ./bear > bear.uml
//}

@<code>{bear.uml}が出力されました。

//cmd{
├── bear
│   ├── bear.go
│   ├── blackbear.go
│   └── polerbear.go
└── bear.uml
//}

@<code>{bear.uml}はPlantUMLの書式であることがわかります。

//emlist[bear.uml]{
@startuml
namespace bear {
    class Bear << (S,Aquamarine) >> {
        + Name string
        + Color string

        + Sleep() 
        + Eat() 

    }
    interface BearInterface  {
        + Sleep() 
        + Eat() 

    }
    class BlackBear << (S,Aquamarine) >> {
        + Climbing() 

    }
    class PolerBear << (S,Aquamarine) >> {
        + Swim() 

    }
}
"bear.Bear" *-- "bear.BlackBear"
"bear.Bear" *-- "bear.PolerBear"

"bear.BearInterface" <|-- "bear.Bear"

@enduml
//}

この@<code>{bear.uml}をPlantUMLで生成したUML図は次のとおりです。Bear、BlackBear、PolerBearの関係性が見える図が出力されました。
//image[002][Bear UML][width=0.5\maxwidth]


===[column] Goの委譲と継承
Goには継承という概念が実装されていません。そのかわり、構造体を埋め込むことで委譲（delegation）を実装することができます。
この章でその説明をすることはありませんが、構造体の埋め込みについては、Effective GoのEmbedding@<fn>{Embedding}に詳しく説明されています。

//footnote[Embedding][@<href>{https://golang.org/doc/effective_go.html#embedding}]

===[/column]

== さいごに
PlantUMLはUMLをテキストで直感的に書くことができる柔軟なツールです。PlantUMLをGoで生成できるようにすることで、Goも設計も楽しい状態をつくり出すことができます。
今後、Goからシーケンス図やER図などを出力できるようになれば、すばらしい未来が待っていることでしょう。

それはまた、別の話。

#@# その他:
#@# https://github.com/steinfletcher/出すpitest-plantuml
#@# https://github.com/yogendra/plantuml-go
#@# https://github.com/kazukousen/gouml

===[column] その他のGo製UMLパッケージ
Go製のUMLパッケージは、@<code>{jfeliu007/goplantuml}以外にもいくつかあります。

 : steinfletcher/apitest-plantuml@<fn>{apitest-plantuml}
    APIテストの結果をPlantUMLの形式で出力できるツールです。

 : yogendra/plantuml-go@<fn>{plantuml-go}
    Javaがインストールされていない環境でも生成可能な環境をホストしてUMLを出力できるツールです。

 : kazukousen/gouml@<fn>{gouml}
    GoのソースコードからPlantUMLのテキストファイルを生成できるツールです。
    Goのfilepathパッケージは次のように出力されます。

//image[003][filepathパッケージ]

//footnote[apitest-plantuml][@<href>{https://github.com/steinfletcher/apitest-plantuml}]
//footnote[plantuml-go][@<href>{https://github.com/yogendra/plantuml-go}]
//footnote[gouml][@<href>{https://github.com/kazukousen/gouml}]

===[/column]
