= LeetCodeでアルゴリズムとデータ構造エクササイズ

@<tt>{@budougumi0617}@<fn>{bd617_twitter}です。
本章ではLeetCodeという競技プログラミングコンテストサービスを紹介します。
LeetCodeを使うとローカルで好みのエディタ・IDEを使って簡単にアルゴリズムとデータ構造を学習することができます。
私自身がコンテストに参加する利用方法をとっていないため、本章ではコンテストの紹介はしません。

//footnote[bd617_twitter][@<href>{https://twitter.com/budougumi0617}]

== LeetCodeとは
LeetCodeはオンライン競技プログラミングコンテストが行われるWebサービスです。

 * @<href>{https://leetcode.com/}


無料@<fn>{free}でサービスを利用することができ、データ構造やアルゴリズムを学べます。
毎週コンテストが行われて世界中の参加者とレーティングを競ったり、過去問にはいつでも挑戦することができます。
過去問には実際に企業の採用面接で出されたコーディング課題もあり@<fn>{leek}、GAFAを目指す方々の面接対策としても使われています@<fn>{google}。
採用面接に使われるような難しい問題ばかりではなく、大学の授業で学ぶ「データ構造とアルゴリズム」レベルの基本的な問題もあります。

//footnote[free][課金しないと見れない回答などはあります。]
//footnote[leek][非公式にリークされた問題もあるようなのでグレーなサイトという指摘もあります。]
//footnote[google][@<href>{https://1kohei1.com/google}]

=== LeetCodeの良いところ・悪いところ
オンライン競技プログラミングコンテストを実施するサービスはいくつか存在します。
その中でもLeetCodeを使ってデータ構造とアルゴリズムを学ぶメリットは次のとおりです。

 * 関数が与えられるので、入出力処理を考える必要がない
 * 関数の入出力を使うのでテストコードが書きやすい
 * 自分の好きなプログラミング言語で挑戦することができる
 * わからなくても解答や問題別掲示板で他の人の解答内容を見て勉強できる
 * 3rdパーティのVSCodeプラグインやCLIツールなどが存在し、ローカルで解きやすい

 デメリットとしは次の点が挙げられます。

 * 日本語未サポート
 * コンテストに参加する場合は時差がある
 
サービスとしては英語オンリー@<fn>{lc_cn}ですが、コードをみたりアルゴリズムをキーワードとして検索すれば英語が苦手な私でも十分理解できています。

次節よりメリットをもう少し詳しく説明します。

//footnote[lc_cn][@<href>{https://leetcode-cn.com/}という中国語版はあるようです]

==== 関数が与えられるので、入出力処理を考える必要がない

 例えば他の競技プログラミングコンテストのWebサービスでは標準入力がインプットとして与えられることが多いです。そのため、@<strong>{文字列をパースしてデータを組み立てる実装が必要}になります。
 同様に、計算結果も標準出力で提出するサービスが多くアルゴリズムではなく、@<strong>{解答をフォーマットに沿った文字列へ加工する実装が必要}になります。
@<strong>{LeetCodeではそのような作業は必要ありません}。LeetCodeの問題は予め関数定義が用意されるので、@<strong>{関数の引数を使って問題を解き、戻り値として解答を返すだけで解けます}。
例えば、次の関数定義は連結リストの問題@<fn>{q_rll}のために、事前に定義された関数です。

//footnote[q_rll][@<href>{https://leetcode.com/explore/interview/card/top-interview-questions-easy/93/linked-list/560/}]

//list[rll][Reverse Linked Listの課題]{
/**
 * Definition for singly-linked list.
 * type ListNode struct {
 *     Val int
 *     Next *ListNode
 * }
 */
func reverseList(head *ListNode) *ListNode {
  // 回答を実装する
}
//}

引数として先頭のリストへのポインタが渡され、処理が終わったら解答としてやはりリストの先頭のポインタを返すだけで解答ができます。リストのデータ構造も既定のものがあるので、アルゴリズムだけに集中して解くことができます。また、選択した言語の通常の関数定義と変わらないので、@<strong>{テストコードが書きやすい}のもメリットです。

==== 自分の好きなプログラミング言語で挑戦することができる
予め関数定義が用意されるので、問題に集中しやすいLeetCodeですが、対応言語も豊富です。

* @<href>{https://support.leetcode.com/hc/en-us/articles/360011833974-What-are-the-environments-for-the-programming-languages-}

2020/09/10現在、LeetCodeは18言語をサポートしています。
@<tt>{C}、@<tt>{C++}、@<tt>{Java}、@<tt>{Python}、@<tt>{Scala}などの主要な言語はもちろん、@<tt>{Rust}や@<tt>{Kotlin}と言った比較的新しい言語でも挑戦することができます。
@<tt>{Go}は1.13を使って解くことができます。

=== わからなくても解答や問題別掲示板で他の人の解答内容を見て勉強できる


* @<href>{https://leetcode.com/explore/interview/card/top-interview-questions-easy/93/linked-list/560/discuss}


==== 3rdパーティのVSCodeプラグインやCLIツールなどが存在し、ローカルで解きやすい
LeetCodeも他の競技プログラミングコンテストサービスと同様にブラウザ上のエディタを使って解答を投稿することができます。
しかし、入力補完を使ったり、テストコードを書いたり@<fn>{leetcode_test}したいときもあります。
また、@<tt>{Git}を利用して解答を管理しておきたいというニーズもあると思います。
LeetCodeにはローカルで解答の雛形の取得、提出などを基本操作を行うOSSが複数存在します。
今回は私が利用したことのあるVSCodeプラグインとRustのCLIツールを紹介したいと思います。



//footnote[leetcode_test][既定のテストケースを数個実行する機能は提供されています]

== OSSを使ってローカルでLeetCodeの課題に挑戦する
各ツールを利用する前に事前に必要なことはLeetCodeのアカウントを取得しておくことです。
GitHubやgmailを使うと簡単にアカウントを発行できます。

 * @<href>{https://leetcode.com/accounts/login/}

また、利用方法によっては次の作業を事前にしておくとよいでしょう。

 * Git管理する場合は、予め@<tt>{git pull}（もしくは@<tt>{git init}）したディレクトリを用意する
 * （VS Codeを使って開発する場合）LeetCodeのアカウントをGitHub連携しておくとログインが簡単


=== VS Codeを使って

 * LeetCode - Visual Studio Marketplace@<fn>{vsc_plugin}

//footnote[vsc_plugin][@<href>{https://marketplace.visualstudio.com/items?itemName=LeetCode.vscode-leetcode}]


=== CLIツールを使って挑戦する
https://docs.rs/leetcode-cli/0.2.25/leetcode_cli/
https://github.com/clearloop/leetcode-cli

//cmd{
$ curl https://sh.rustup.rs -sSf | sh
$ rustup override set nightly
info: using existing install for 'nightly-x86_64-apple-darwin'
info: override toolchain for '/Users/yoichiroshimizu' set to 'nightly-x86_64-apple-darwin'

  nightly-x86_64-apple-darwin unchanged - rustc 1.48.0-nightly (5099914a1 2020-09-08)
$ rustc -V
rustc 1.48.0-nightly (5099914a1 2020-09-08)
//}

//cmd{
$ leetcode -h
leetcode-cli 0.2.25
May the Code be with You 👻

USAGE:
    leetcode [FLAGS] [SUBCOMMAND]

FLAGS:
    -d, --debug      debug mode
    -h, --help       Prints help information
    -V, --version    Prints version information

SUBCOMMANDS:
    data    Manage Cache [aliases: d]
    edit    Edit question by id [aliases: e]
    exec    Submit solution [aliases: x]
    list    List problems [aliases: l]
    pick    Pick a problem [aliases: p]
    stat    Show simple chart about submissions [aliases: s]
    test    Test question by id [aliases: t]
    help    Prints this message or the help of the given subcommand(s)

//}

//list[bear/bear.go][hoge]{
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
	fmt.Println("眠る")
}

func (b *Bear) Eat() {
	fmt.Println("食べる")
}
//}


== どの問題をやればいいの？
1400問以上存在します。
 * Easy Collection | Top Interview Questions@<fn>{top_interview}
 * コーディング面接対策のために解きたいLeetCode 60問@<fn>{1kohei1}

//footnote[top_interview][@<href>{https://leetcode.com/explore/interview/card/top-interview-questions-easy/}]
//footnote[1kohei1][@<href>{https://1kohei1.com/leetcode/}]