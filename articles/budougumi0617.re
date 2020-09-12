= LeetCodeでアルゴリズムとデータ構造エクササイズ

@<tt>{@budougumi0617}@<fn>{bd617_twitter}です。
本章ではLeetCodeという競技プログラミングコンテストサービスを紹介します。
LeetCodeはアルゴリズムとデータ構造を学習することができます。
そして本章ではローカルでテスト駆動開発（TDD）を用いながらLeetCodeに挑戦する方法をまとめます。
私自身がコンテストに参加する利用方法をとっていないため、本章ではコンテストの紹介はしません。
なお、本章内のディレクトリ構成やショートカット操作はすべてmacOS上を前提とします。

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

=== LeetCodeのよいところ・悪いところ
オンライン競技プログラミングコンテストを実施するサービスはいくつか存在します。
その中でもLeetCodeを使ってデータ構造とアルゴリズムを学ぶメリットは次のとおりです。

 * 関数が与えられるので、入出力処理を考える必要がない。
 * 関数の入出力を使うのでテストコードが書きやすい。
 * 自分の好きなプログラミング言語で挑戦することができる。
 * わからなくても解答や問題別掲示板で他の人の解答内容を見て勉強できる。
 * 3rdパーティのVisual Studio Code（VSCode）プラグインやCLIツールなどが存在し、ローカルで解きやすい。

 デメリットとしては次の点が挙げられます。

 * 言語設定がないので英語オンリー。
 * 日本からコンテストに参加する場合は時差があるので深夜開始になったりする。
 
サービスとしては英語オンリー@<fn>{lc_cn}ですが、コードをみたりアルゴリズムをキーワードとして検索すれば英語が苦手な私でも十分理解できるレベルです。

次節よりメリットをもう少し詳しく説明します。

//footnote[lc_cn][@<href>{https://leetcode-cn.com/}という中国語版はあるようです]

==== 関数が与えられるので、入出力処理を考える必要がない

 たとえば他の競技プログラミングコンテストのWebサービスでは標準入力がインプットとして与えられることが多いです。そのため、@<strong>{文字列をパースしてデータを組み立てる実装が必要}になります。
 同様に、計算結果も標準出力で提出するサービスが多くアルゴリズムではなく、@<strong>{解答をフォーマットに沿った文字列へ加工する実装が必要}になります。
@<strong>{LeetCodeではそのような作業は必要ありません}。LeetCodeの問題はあらかじめ関数定義が用意されるので、@<strong>{関数の引数を使って問題を解き、戻り値として解答を返すだけで解けます}。
たとえば、次の関数定義は連結リストの問題@<fn>{q_rll}を解くときに与えられる関数定義です。

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

引数として先頭のリストへのポインタが渡され、処理が終わったら解答としてやはりリストの先頭のポインタを返すだけで解答ができます。リストのデータ構造も既定のものがあるので、アルゴリズムだけに集中して解くことができます。また、選択した言語の通常の関数定義と変わらないので、@<strong>{テストコードが書きやすく、TDDで解きやすい}のもメリットです。

==== 自分の好きなプログラミング言語で挑戦することができる
あらかじめ関数定義が用意されるので、問題に集中しやすいLeetCodeですが、対応言語も豊富です。

 * What are the environments for the programming languages?@<fn>{support_lang}

//footnote[support_lang][@<href>{https://bit.ly/2R9Zrok}]

2020/09/10現在、LeetCodeは18言語をサポートしています。
@<tt>{C}、@<tt>{C++}、@<tt>{Java}、@<tt>{Python}、@<tt>{Scala}などの主要な言語はもちろん、@<tt>{Rust}や@<tt>{Kotlin}と言った比較的新しい言語でも挑戦することができます。
@<tt>{Go}は1.13を使って解くことができます。

==== わからなくても解答や問題別掲示板で他の人の解答内容を見て勉強できる
問題によってはどうアプローチをすればよいのか、まったくわからないときもあります。
そのようなときでもLeetCodeは解答を確認したり、問題別の掲示板を見ることで他の参加者の意見を確認することができます。

 * Discussion for Reverse Linked List@<fn>{560_discuss}

//footnote[560_discuss][@<href>{https://leetcode.com/explore/interview/card/top-interview-questions-easy/93/linked-list/560/discuss}]


==== 3rdパーティのVSCodeプラグインやCLIツールなどが存在し、ローカルで解きやすい
LeetCodeも他の競技プログラミングコンテストサービスと同様にブラウザ上のエディタを使って解答を投稿することができます。
しかし、入力補完を使ったり、テストコードを書いたり@<fn>{leetcode_test}したいときもあります。
また、@<tt>{Git}を利用して解答を管理しておきたいというニーズもあると思います。
LeetCodeにはローカルで解答の雛形の取得、提出などを基本操作を行うOSSが複数存在します。
今回は私が利用したことのあるVSCodeプラグインと@<tt>{Rust}のCLIツールを紹介したいと思います。

//footnote[leetcode_test][既定のテストケースを数個実行する機能は提供されています]

== OSSを使ってローカルでLeetCodeの課題に挑戦する
各ツールを利用する前に事前に必要なことはLeetCodeのアカウントを取得しておくことです。
GitHubやgmailを使うと簡単にアカウントを発行できます。

 * @<href>{https://leetcode.com/accounts/login/}

また、利用方法によっては次の作業を事前にしておくとよいでしょう。

 * Git管理する場合は、あらかじめ@<tt>{git pull}（もしくは@<tt>{git init}）したディレクトリを用意する。
 * （VSCodeを使って開発する場合）LeetCodeのアカウントをGitHub連携しておく。
 ** ログインが簡略化できます。

=== VSCodeを使ってLeetCodeに挑戦する
VSCodeを使ってLeetcodeの問題を解く場合、VSCodeプラグインを利用することができます。

 * LeetCode - Visual Studio Marketplace@<fn>{vsc_plugin}

//footnote[vsc_plugin][@<href>{https://marketplace.visualstudio.com/items?itemName=LeetCode.vscode-leetcode}]

VSCodeプラグインを使うと、VSCodeで次のような操作ができるようになります。

 * 問題を一覧、問題文を確認する。
 * 問題を難易度やジャンルで検索する
 * 選択した問題の提出用雛形コードをローカルに生成する。
 * 実装したコードを使ってオンラインサーバのテストを実施する。
 * 実装したコードをオンラインサーバへ提出する。

==== プラグインの初期設定

プラグインをインストールしてVSCode上でLeetCodeへログインします。このときGitHubやgmailを使ってログインができると簡単です。
VSCode上で@<tt>{⌘+Shift+P}を押下し、@<kw>{Open User Settings}を選択します。
その後、@<kw>{設定の検索}で@<tt>{leetcode}を検索します。
@<kw>{LeetCode: Default Languages}で@<tt>{golang}を選択しておきます。
LeetCodeプラグインが生成するファイルはデフォルト設定だとホームディレクトリ配下の隠しフォルダに保存されていきます。


もし、Git管理したディレクトリで解答コードを管理したい場合は、@<kw>{LeetCode: Workspace Folder}のディレクトリ設定を変更しておく必要があります@<fn>{vscode_dir}。

//footnote[vscode_dir][例: @<tt>{/Users/budougumi0617/go/src/github.com/budougumi0617/leetcode}]

==== TDDでLeetCodeの問題を解く
LeetCode Pluginで解答用コードを自動生成しても、補完などが効かなかったりします。
これは解答用コードが@<tt>{Go}のコードとして正しくないため、Language Serverが動かないためです。
VSCodeの@<tt>{Go}プラグインのサポートを借りながらコーディングするには、解答用コードに@<tt>{package}名を追加する必要があります。
これによって、課題が提出できなくなるようなことはありません。
@<tt>{package}名を追加することで、テストコードの自動生成も実現できます。
解答用コードにファイル先頭に@<tt>{package main}と追記したあと、VSCode上で@<tt>{⌘+Shift+P}を押下し、@<kw>{Go: Generate Unit Tests For File}を実行すればテストコードファイルが生成されます。
あとは問題文に記載された@<list>{125_example}のような例題をテストケースに実装していけばTDDを開始できます。

//list[125_example][問題文に書かれたExample]{
 * Example 1:
 * Input: "A man, a plan, a canal: Panama"
 * Output: true
 *
 * Example 2:
 * Input: "race a car"
 * Output: false
//}

//list[125_example_test][テストコードにしてTDD]{
  tests := []struct {
    name string
    args args
    want bool
      {
         name: "true",
         args: args{s: "A man, a plan, a canal: Panama"},
         want: true,
      },
      {
         name: "false",
         args: args{s: "race a car"},
         want: false,
      },
  }

  for _, tt := range tests {
    tt := tt
    t.Run(tt.name, func(t *testing.T) {
      if got := isPalindrome(tt.args.s); got != tt.want {
        t.Errorf("isPalindrome() = %v, want %v", got, tt.want)
      }
    })
  }
//}


また、submitしたときに実装ミスでテストをPASSできないことがあります。

//list[125_fail][submit時にテスト不合格だったときの結果出力]{
Wrong Answer
458/481 cases passed (N/A)
Testcase
"0P"
Answer
true
Expected Answer
false
//}

そのようなときもその通過できなかったテストケースをテストコードに追加して実装を再開できます。

//list[125_add_case][テストケースを追加して再開]{
  tests := []struct {
    name string
    args args
    want bool
  }{
      // oher cases...
      {
         name: "false2",
         args: args{s: "0P"},
         want: false,
      },
  }
//}


=== CLIツールを使って挑戦する
VSCode以外のエディタ、IDEを利用する場合はCLIを使ったほうが便利でしょう。
3rdパーディ製のRustで実装された@<tt>{leetcode}コマンドが存在します。

 * @<href>{https://docs.rs/leetcode-cli/0.2.25/leetcode_cli/}
 * @<href>{https://github.com/clearloop/leetcode-cli}

インストール手順は次のとおりです。インストール中のコンパイルで失敗する場合、Rustのリリースチャネルを@<tt>{nightly}に切り替える必要がある可能性があります。


//cmd{
$ curl https://sh.rustup.rs -sSf | sh
$ rustup override set nightly
info: using existing install for 'nightly-x86_64-apple-darwin'
info: override toolchain for '/Users/yoichiroshimizu' set to \
'nightly-x86_64-apple-darwin'

  nightly-x86_64-apple-darwin unchanged - \
rustc 1.48.0-nightly (5099914a1 2020-09-08)
$ rustc -V
rustc 1.48.0-nightly (5099914a1 2020-09-08)
//}

用意されているサブコマンドは次のとおりです。VSCodeで利用できる操作はカバーされているはずです。

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

@<tt>{leetcode}コマンドはTOMLファイルで設定を管理しています。
認証情報、言語設定とコードの保存ディレクトリを設定しておく必要があります。
認証情報の取得方法はGitHub上のREADME@<fn>{cli_cookies} で確認することができます。
デフォルト言語設定を@<tt>{Go}、エディタをVimに設定すると@<list>{cli_setting}のようになります。

//footnote[cli_cookies][@<href>{https://github.com/clearloop/leetcode-cli#cookies}]

//list[cli_setting][TOMLファイルの設定抜粋]{
[code]
editor = 'vim'
lang = 'golang'

[cookies]
csrf = "7..."
session = "eyJ...."

[storage]
code = '/Users/budougumi0617/go/src/github.com/budougumi0617/leetcode'
//}

==== CLI利用時にテストコードを自動生成する
CLIでは@<tt>{leetcode list}コマンドで解きたい問題を探し、@<tt>{leetcode edit $NUM}で課題提出用のコードを生成します。
CLIで生成するコードでも、@<tt>{package}名が設定されていないコードファイルが生成されるのでそのままでは動きません。

//list[cli_edit][自動生成されるコード]{
/**
 * Definition for singly-linked list.
 * type ListNode struct {
 *     Val int
 *     Next *ListNode
 * }
 */
func addTwoNumbers(l1 *ListNode, l2 *ListNode) *ListNode {

}
//}

ファイルを開いたら冒頭に@<tt>{package main}を追記しましょう。

VSCodeで紹介したようにテストコードを自動生成したい場合は@<tt>{gotests}コマンドを使います@<fn>{gotests}。

 * @<href>{https://github.com/cweill/gotests}

//footnote[gotests][VSCocdeも内部的には@<tt>{gotests}を使ってテストコードを生成しています]

インストール手順はGitHub上のREADMEのとおりです。

//cmd{
$ go get -u github.com/cweill/gotests/...
//}

一番簡単な利用方法としてはLeetCodeに提出する課題定義があるディレクトリで次のように実行します。

//cmd{
$ gotests -all ...
//}

これで任意のエディタ、IDEを使ってLeetCodeに挑戦する準備ができました。


== どの問題をやればいいの？
LeetCodeにはすでに1400問以上の問題が存在します。
どの問題を解くか迷う方は次のコレクションを参考にするといいでしょう。

 * Easy Collection | Top Interview Questions@<fn>{top_interview}
 * コーディング面接対策のために解きたいLeetCode 60問@<fn>{1kohei1}

//footnote[top_interview][@<href>{https://leetcode.com/explore/interview/card/top-interview-questions-easy/}]
//footnote[1kohei1][@<href>{https://1kohei1.com/leetcode/}]


== 終わりに
本章ではLeetCodeの概要と、LeetCodeを使った学習をローカルで行なう方法を紹介しました。
本当はコマンド1回で提出用コードとテストコードを一緒に生成してすぐTDDでLeetCodeを解けるCLIツールを@<tt>{Go}で作ろうとしたのですが、本書には間に合いませんでした。
近いうちに公開してTwitter@<fn>{bd617_twitter2}かブログ@<fn>{bd617_blog}で告知するつもりなので、興味があったらチェックお願いします。

//footnote[bd617_twitter2][@<href>{https://twitter.com/budougumi0617}]
//footnote[bd617_blog][@<href>{https://budougumi0617.github.io/}]

