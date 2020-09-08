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
今回の記事を読んでステガノグラフィーに興味がでた人は、ネットで検索するといろいろ出てくるので参考にしてみてください。


