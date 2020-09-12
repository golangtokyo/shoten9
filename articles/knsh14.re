= パーサを作って学ぶiCalendar

本章ではiCalendar形式をパースし、Goのオブジェクトに変換するライブラリの作成をします。
その過程でiCalendar形式の仕様に対しての理解を深めていきます。

本章で解説するパーサの実装はGitHub@<fn>{github_repo_link}で公開しています。

//footnote[github_repo_link][@<href>{https://github.com/knsh14/ical}]

== Goの実行環境
OSはmacOS 10.14.6です。
Goのバージョンはgo1.14 darwin/amd64です。

== 概要
iCalendar形式はRFC 5545@<fn>{rfc_5545_link}で詳細な仕様が定義されています。

RFC 5545にしたがって記述されたiCalendar形式のファイルの内容をGoで扱うためには、Goの構造体のインスタンスに変換する必要があります。
Goの構造体にするためには、ファイルの文字列を解釈し変換します。

この操作は言語処理系の実装における字句解析、構文解析と同じ考えを用いて実装できます。
まずiCalendar形式のファイルを字句解析します。
字句解析で得られた字句を構文解析し、iCalendarの構造体へ変換します。

@<list>{example_input}の情報が記述されたファイルを@<list>{example_output}へ変換します。

//list[example_input][iCalendar形式のファイルの中身]{
BEGIN:VCALENDAR
PRODID:-//Google Inc//Google Calendar 70.9054//EN
VERSION:2.0
END:VCALENDAR
//}

//list[example_output][変換されたiCalendarのオブジェクト][go]{
&ical.Calender{
    ProdID: &property.ProdID{
      Parameter: parameter.Container{},
      Value:     "-//Google Inc//Google Calendar 70.9054//EN",
    }
    Version: &property.Version{
        Parameter: parameter.Container{},
        Max:       types.Text("2.0"),
    },
},
//}

//footnote[rfc_5545_link][@<href>{https://tools.ietf.org/html/rfc5545}]

== iCalendarファイルの形式
iCalendar形式のファイルは次の特徴があります。

 1. 文字コードはUTF-8でなければいけません。@<fn>{ical_character_set}
 2. コンテンツラインという単位で情報が記述されます。
 3. コンポーネントと呼ばれるオブジェクトの入れ子構造で構成されています。
 4. コンポーネントは複数のコンテンツラインで構成されています。
 5. コンポーネントの始まりと終わりを表すコンテンツラインと、コンポーネントのプロパティの情報を表すコンテンツラインがあります。

//footnote[ical_character_set][@<href>{https://tools.ietf.org/html/rfc5545#section-3.1.4}]

=== コンテンツライン
コンテンツラインはiCalendarの情報を記述する単位です。

@<list>{content_line_style}はコンテンツラインの形式です。

//list[content_line_style][コンテンツラインの形式]{
NAME;PARAM:VALUE
//}

@<tt>{NAME}はコンテンツラインの種類を示します。
コンポーネントの始まりと終わりや、それぞれのコンポーネントのプロパティの種類などがあります。

@<tt>{VALUE}は@<tt>{NAME}に対応する値です。
@<tt>{VALUE}は@<tt>{NAME}の項目によっては複数の@<tt>{VALUE}を設定できます。
複数の@<tt>{VALUE}はカンマで区切る必要があります。

コンテンツラインには少なくとも@<tt>{NAME}と@<tt>{VALUE}の2つがなければいけません。

@<tt>{PARAM}は必須でない項目です。
@<tt>{NAME}に対応する@<tt>{VALUE}にさらに情報を足すために使われます。
たとえば、イベントの開始時刻のタイムゾーンや、複数のデータ型が使えるプロパティでデータの型名を明示したりする場合に用います。

コンテンツラインは基本的にはファイル内の各行がひとつのコンテンツラインになります。
ただし1行は75オクテット以内でなければいけません。@<fn>{content_line_overview}
しかし、イベントの説明などの場合にこの制限を超える場合があります。
その場合にはコンテンツラインを複数行に分割する必要があります。
複数行に分割する際に2行目以降は行の先頭にスペース、もしくはタブ文字を挿入します。
余談ですが、世の中に出回っているiCalendarファイルにはこの規約を守っていないものも多くあります。

//footnote[content_line_overview][@<href>{https://tools.ietf.org/html/rfc5545#section-3.1}]

=== コンポーネント
iCalendar形式をパースする目的はこのコンポーネントのオブジェクトの生成です。
コンポーネントには次の種類が定義されています。

 * Calendarコンポーネント
 * Eventコンポーネント
 * Timezoneコンポーネント
 * ToDoコンポーネント
 * Journalコンポーネント
 * FreeBusyコンポーネント
 * Standardコンポーネント
 * Daylightコンポーネント
 * Alarmコンポーネント

これらのコンポーネントが木構造になっています。
根のコンポーネントとして使えるのはCalendarコンポーネントのみです。
Eventコンポーネント、ToDoコンポーネントにはAlarmコンポーネントが含まれる場合があります。
TimezoneコンポーネントにはサブコンポーネントとしてStandardコンポーネント、Daylightコンポーネントが含まれます。

それぞれのコンポーネントはプロパティを持っています。
CalendarコンポーネントはiCalのバージョンを示すプロパティを持っています。
Eventコンポーネントはイベントのタイトルや、終了時刻を持っています。
なお、プロパティには必須の項目、必須ではないが、コンポーネント中にひとつしかセットできない項目、0個以上設定できる項目があります。

== iCalendar形式のファイルをパースする流れ

iCalendar形式のファイルをパースしてGoのオブジェクトに変換するためには、いくつかのステップを経由して変換します。

 1. 文字列をコンテンツラインに変換する
 2. コンテンツラインからコンポーネントのオブジェクトに変換する


=== 文字列をコンテンツラインに変換する
前半のステップとして、iCalendar形式のファイルの中身をコンテンツラインに変換します。
入力の例を@<list>{input_raw_data}に、@<list>{input_raw_data}のデータを処理して得られるコンテンツラインを@<list>{expected_output_content_line}に示します。

//list[input_raw_data][想定される入力]{
BEGIN:VEVENT
DTSTART;VALUE=DATE:20211011
DTEND;VALUE=DATE:20211012
DTSTAMP:20200716T144532Z
UID:20211011_60o30d9l6ko32e1g60o30dr56k@google.com
SUMMARY:体育の日
END:VEVENT
//}

//list[expected_output_content_line][期待する出力結果であるコンテンツラインのスライス][go]{
type ContentLine struct {
  Name       string
  Parameters []Parameter
  Values     []string
}

type Parameter struct {
    Name   string
    Values []string
}

[]*ContentLine{
  {
    Name: "BEGIN",
    Parameter: nil,
    Values: []string{"VEVENT"},
  },
  {
    Name: "DTSTART",
    Parameter: []Parameter{
      {
        Name: "VALUE",
        Values: []string{"DATE"}
      },
    },
    Values: []string{"20211011"},
  },
  {
    Name: "DTEND",
    Parameter: []Parameter{
      {
        Name: "VALUE",
        Values: []string{"DATE"}
      },
    },
    Values: []string{"20211012"},
  },
  {
    Name: "DTSTAMP",
    Parameter: nil,
    Values: []string{"20200716T144532Z"},
  },
  {
    Name: "UID",
    Parameter: nil,
    Values: []string{"20211011_60o30d9l6ko32e1g60o30dr56k@google.com"},
  },
  {
    Name: "SUMMARY",
    Parameter: nil,
    Values: []string{"体育の日"},
  },
  {
    Name: "END",
    Parameter: nil,
    Values: []string{"VEVENT"},
  },
}
//}

@<list>{input_raw_data}から@<list>{expected_output_content_line}への変換を実現するには、次のステップで実現します。

 1. 複数行をひとつの文字列オブジェクトとして分割する
 2. 文字列から字句を取り出す字句解析器を生成する
 3. 字句解析器から字句を取り出しコンテンツラインに変換する

==== 複数行をひとつの文字列オブジェクトとして分割する

コンテンツラインは複数行でひとつのコンテンツラインを形成する場合があります。
そのため、まずすべての行をチェックし、先頭の文字を確認します。
もし、スペースもしくはタブ文字なら複数行のコンテンツラインの一部と見なします。
最初の一文字を削り前の行と結合して、ひとつのコンテンツラインになる文字列とします。

@<list>{build_raw_lines_to_string_slice}に実際のコードを示します。
@<code>{bufio.Scanner}型を用いて1行ずつ読み込みながら、各行に対して先頭がスペースもしくはタブ文字かチェックを行います。
すべてのチェックが終わると、コンテンツラインになる文字列のスライスに変換されます。
このスライスの各要素に対してコンテンツラインへの変換します。

//list[build_raw_lines_to_string_slice][文字列の集合から行ごとの文字列のスライスへの変換][go]{
var res []string
for scanner.Scan() {
    l := scanner.Text()
    switch {
    case strings.HasPrefix(l, " "):
        res[len(res)-1] += "\n" + strings.TrimPrefix(l, " ")
    case strings.HasPrefix(l, "\t"):
        res[len(res)-1] += "\n" + strings.TrimPrefix(l, "\t")
        continue
    default:
        res = append(res, l)
    }
}
//}

==== 文字列から字句を取り出す字句解析器を生成する
@<tt>{DTSTART:19980118T073000Z}という文字列を@<list>{expected_generated_tokens}に示す字句の集合に変換します。
字句は単なる文字列を意味のある単位にまとめたものです。
英語圏ではトークンと呼びます。
コード上でも@<tt>{Token}と表現します。
たとえば@<list>{expected_generated_tokens}の1つ目の要素は@<tt>{IDENT}という識別子を表す字句です。
その@<tt>{IDENT}という種類の字句が@<tt>{DTSTART}という値を持っています。
文字列から字句の列へ変換する処理を字句解析と呼びます。

//list[expected_generated_tokens][生成したい字句の集合][go]{
type Token struct {
    Type  Type
    Value string
}
token.Token{Type: token.IDENT, Value: "DTSTART"}
token.Token{Type: token.COLON, Value: ":"}
token.Token{Type: token.IDENT, Value: "19980118T073000Z"}
token.Token{Type: token.EOF, Value: ""}
//}

文字列からコンテンツラインへ変換する際に、字句への変換を挟むと変換処理が簡単になります。
もし、字句を使わずに正規表現で変換しようとすると、うまく行かないパターンが出てきます。
例として、RFC 5545から引用した例@<fn>{example_hard_parsing_link}@<list>{example_input_hard_parsing}から@<list>{example_output_hard_parsing}の変換を考えます。

//list[example_input_hard_parsing][正規表現などでコンテンツラインへ変換するのが難しい内容]{
ATTENDEE;DELEGATED-FROM="mailto:jsmith@example.com":mailto:jdoe@example.com
//}

//list[example_output_hard_parsing][@<list>{example_input_hard_parsing}から期待する変換結果][go]{
[]*ContentLine{
  {
    Name: "ATTENDEE",
    Parameter: []Parameter{
      {
        Name: "DELEGATED-FROM",
        Values: []string{"mailto:jsmith@example.com"}
      },
    },
    Values: []string{"mailto:jdoe@example.com"},
  },
}
//}

//footnote[example_hard_parsing_link][@<href>{https://tools.ietf.org/html/rfc5545#section-3.2.4}]

この場合、最初に出てきたコロンを@<tt>{NAME}と@<tt>{VALUE}の区切りとしてしまうと@<tt>{mailto:jsmith@example.com}という値を取得できません。
また、@<tt>{ATTENDEE}に対応する値が、@<tt>{jsmith@example.com":mailto:jdoe@example.com}となり、これも正しくありません。
正しく分割するためには、先頭から一文字ずつチェックしていき、@<tt>{NAME}なのか、@<tt>{VALUE}なのか、その中でもダブルクオートの中の文字列なのか判断しながら区切っていく必要があります。
この処理は正規表現や@<tt>{strings}パッケージの@<code>{Split}関数では実現できないため、字句解析を用います。

字句解析の考え方は単純です。
先頭から一文字ずつ文字を読んでいき、適切な字句に振り分けていきます。
iCalendarを字句解析するために必要な字句の種類は次の5つです。

  1. 文字列
  2. コロン
  3. セミコロン
  4. イコール
  5. カンマ
  6. 終端

文字列とは@<tt>{NAME}や@<tt>{VALUE}の値になる要素です。
コロン、セミコロンは@<tt>{NAME}と@<tt>{PARAM}、@<tt>{VALUE}の区切りを示す字句です。
イコールは@<tt>{PARAM}内での名前と値の区切りを示します。
カンマは@<tt>{VALUE}、@<tt>{PARAM}の値が複数ある場合に区切りを示す字句です。
終端はコンテンツラインの終端です。
この字句があることによって、字句のスライスからコンテンツラインへの変換が少し見やすくなります。

@<list>{definition_and_initial_lexer}に字句解析器の定義と初期化の実装を示します。

//list[definition_and_initial_lexer][字句解析器の定義と初期化][go]{
type Lexer struct {
    input        []rune // 入力値
    position     int // 字句解析が完了した位置
    readPosition int // 現在字句解析している位置
    ch           rune // 現在字句解析する文字

    checkFunc func(rune) bool // 文字列かどうか判定する関数
}

func New(input string) *Lexer {
    l := &Lexer{input: []rune(input), checkFunc: isName}
    l.readChar()
    return l
}

func (l *Lexer) readChar() {
    if l.readPosition >= len(l.input) {
        l.ch = 0
    } else {
        l.ch = l.input[l.readPosition]
    }
    l.position = l.readPosition
    l.readPosition++
}
//}

字句解析器は入力の文字列、現在の字句解析器が読み終わった位置、先読みしている場所の位置、そして現在のチェックする文字を持っています。
先読みしている位置と現在の位置を別に持つことで、文字列の字句を作成するときに便利になるため保存しています。

@<code>{checkFunc}フィールドは文字列判定のための関数を保持しています。
@<tt>{NAME}と@<tt>{VALUE}と@<tt>{PARAM}では文字列として認識できる文字が異なります。@<fn>{acceptable_character_definition}
それぞれを読んでいく際に正しい文字列かチェックする関数を変更できるようにフィールドに持たせています。

//footnote[acceptable_character_definition][@<href>{https://tools.ietf.org/html/rfc5545#section-3.1}]

@<code>{readChar}メソッドは一文字読み進めるメソッドです。
字句解析する際に先頭の文字を取得できるように、初期化する際に一文字読み進めておきます。

字句解析器が字句のスライスに変換するためのメソッドの実装を@<list>{implementation_next_token}に示します。

//list[implementation_next_token][字句を返す関数の実装][go]{
func (l *Lexer) NextToken() token.Token {
    var tok token.Token

    l.skipWhitespace()

    switch l.ch {
    case '=':
        tok = newToken(token.ASSIGN, l.ch)
        l.checkFunc = isParamValue
    case ';':
        tok = newToken(token.SEMICOLON, l.ch)
        l.checkFunc = isParamName
    case ',':
        tok = newToken(token.COMMA, l.ch)
    case '"':
        tok.Type = token.STRING
        tok.Value = l.readString()
    case ':':
        tok = newToken(token.COLON, l.ch)
        l.checkFunc = isValue
    case 0:
        tok.Value = ""
        tok.Type = token.EOF
    default:
        if l.checkFunc(l.ch) {
            tok.Value = l.readIdentifier()
            tok.Type = token.IDENT
            return tok
        }
        tok = newToken(token.ILLEGAL, l.ch)
    }

    l.readChar()
    return tok
}

func (l *Lexer) readIdentifier() string {
    position := l.position
    for l.checkFunc(l.ch) {
        l.readChar()
    }
    return string(l.input[position:l.position])
}

func (l *Lexer) readString() string {
    position := l.position + 1
    for {
        l.readChar()
        if !isDoubleQuoteSafeLetter(l.ch) {
            break
        }
    }

    return string(l.input[position:l.position])
}
//}

字句解析器の@<code>{NextToken}メソッドは現在の字句を返します。
そして現在の字句解析が済んでいる位置を進めます。

どの字句を返すかは現在の文字で分岐し、それにあった字句を返します。
ダブルクオート、もしくはコロン、セミコロン、カンマ、イコール、終端文字以外の文字の場合は文字列を返す処理を行います。

文字列はダブルクオートで囲まれた文字列とそれ以外では、使用できる文字が異なります。
ダブルクオートで囲われた文字列の処理は専用の文字列判別ルールを用いて処理します。
それ以外の文字列は状態によって判定基準が変わります。
@<tt>{NAME}と@<tt>{VALUE}では使える文字の種類が異なるからです。
字句解析器がフィールドとして持っている文字列チェック用の関数を用いて文字列チェックを行い、文字列が続く限り読み進めていきます。


==== 字句からコンテンツラインに変換する
字句解析器から字句が逐次取り出せました。
字句解析の最後のステップとして字句解析器から取り出された字句を元にコンテンツラインを生成します。

//list[implementation_convert_content_line][コンテンツラインへの変換する処理の実装][go]{
type ContentLine struct {
    Name       string
    Parameters []Parameter
    Values     []string
}

type Parameter struct {
    Name   string
    Values []string
}

func ConvertContentLine(l *lexer.Lexer) (*ContentLine, error) {
    var cl ContentLine

    // NAMEを取得する
    n, t, err := getName(l)
    if err != nil {
        return nil, fmt.Errorf("failed to get name: %w", err)
    }
    cl.Name = n

    // もし現在の字句がSEMICOLONならPARAMを取得する
    for t.Type == token.SEMICOLON {
        p, token, err := getParameter(l)
        if err != nil {
            return nil, fmt.Errorf("failed to get parameter: %w", err)
        }
        t = token
        cl.Parameters = append(cl.Parameters, p)
    }

    // EOFが続くまでVALUEを取得する。
    // VALUEは複数列になっている場合があるため、終わるまで行う
    if t.Type != token.COLON {
        return nil, fmt.Errorf("expected \":\" but got %s[%s]",
            t.Type,
            t.Value,
        )
    }
    for t.Type != token.EOF && t.Type != token.ILLEGAL {
        v, token, err := getValue(l)
        if err != nil {
            return nil, fmt.Errorf("failed to get value: %w", err)
        }
        t = token
        cl.Values = append(cl.Values, v)
    }
    if t.Type == token.ILLEGAL {
        return nil, fmt.Errorf("received ILLEGAL %v", t.Value)
    }
    return &cl, nil
}
//}

@<list>{content_line_style}でも示したように、コンテンツラインは@<tt>{NAME}、もしあれば@<tt>{PARAM}、@<tt>{VALUE}の順に並んでいます。

初めに@<tt>{NAME}を変換します。
具体的な実装は@<list>{implementation_get_name}に示します。
@<tt>{NAME}として使えるのは文字列だけです。
そのため文字列が続く限り連結し@<tt>{NAME}の値とします。
本来@<tt>{NAME}として使える文字はアルファベット、数字、ハイフンのみです。
ですがそれは字句解析中にチェックしているため、再度チェックは行いません。
@<tt>{NAME}が終わるのはコロン、セミコロンの場合のみです。
コロンもしくはセミコロンの場合は正しく変換が終了したとみなし、連結した値を返します。
文字列、コロン、セミコロン以外の字句があった場合、不適切な字句としてエラーを返します。

//list[implementation_get_name][コンテンツラインの名前を取得する処理][go]{
func getName(l *lexer.Lexer) (string, token.Token, error) {
    var n string
    for {
        t := l.NextToken()
        switch t.Type {
        case token.IDENT:
            n += t.Value
        case token.SEMICOLON, token.COLON:
            return n, t, nil
        default:
            return "", t, fmt.Errorf("invalid token %s", t.Value)
        }
    }
}
//}

次に@<tt>{PARAM}の変換します。
@<tt>{PARAM}は@<tt>{;KEY=VALUE[,VALUE]}の繰り返しで記述されます。
@<list>{implementation_convert_content_line}のN行目の処理が大まかに@<tt>{PARAM}かどうかをチェックしています。
もしセミコロンがなければ、その時点で@<tt>{PARAM}がないと分かります。
なので@<code>{for}文でセミコロンが見つかる限りは@<tt>{PARAM}に変換します。
具体的な変換処理を@<list>{implementation_get_parameter}に示します。

//list[implementation_get_parameter][コンテンツラインのパラメータを取得する処理][go]{
type Parameter struct {
    Name   string
    Values []string
}

func getParameter(l *lexer.Lexer) (Parameter, token.Token, error) {
    var p Parameter
    for t := l.NextToken(); t.Type != token.ASSIGN; t = l.NextToken() {
        t := l.NextToken()
        switch t.Type {
        case token.IDENT:
            p.Name += t.Value
        case token.ASSIGN:
        default:
            return Parameter{}, t, fmt.Errorf("invalid token %s", t.Value)
        }
    }
    var val string
    for {
        t := l.NextToken()
        switch t.Type {
        case token.IDENT, token.STRING:
            val += t.Value
        case token.COMMA:
            p.Values = append(p.Values, val)
            val = ""
        case token.COLON, token.SEMICOLON:
            p.Values = append(p.Values, val)
            return p, t, nil
        default:
            return Parameter{}, t, fmt.Errorf("invalid token %s", t.Value)
        }
    }
}
//}

@<tt>{PARAM}は@<tt>{KEY=VALUE[,VALUE]}という形式です。
最初に@<tt>{KEY}に相当する文字列を取得します。
これはコンテンツラインの実装と同じように取得していきます。
イコールが現れるまではパラメータ名とみなします。

値の変換は少々複雑です。
値は2つ以上持つことも可能ですので、スライスで保持します。
字句がコロン、セミコロンの場合はその時点で@<tt>{PARAM}の切れ目ですので、変換を終了します。
カンマの場合は値どうしの区切りですので、その時点での文字列を値のスライスに追加し、文字列変数を空にします。
終了や不正な字句が出てきた場合はエラーを返します。


最後に@<tt>{VALUE}へ変換します。
@<list>{implementation_convert_content_line}では値を変換する前にコロンの字句があるかチェックしています。
コロンの字句がないというのはコンテンツラインの仕様ではあってはい けないため、ない場合はエラーを返す必要があります。
コロンの字句のチェックを通過したら、終了字句まで値を読み込み続けます。
値を取得するための実装は@<list>{implementation_get_value}に示します。

//list[implementation_get_value][コンテンツラインから値を取得する処理][go]{
func getValue(l *lexer.Lexer) (string, token.Token, error) {
    var val string
    for {
        t := l.NextToken()
        switch t.Type {
        case token.IDENT, token.STRING:
            val += t.Value
        case token.COMMA, token.EOF:
            return val, t, nil
        default:
            return "", t, fmt.Errorf("invalid token %s", t.Value)
        }
    }
}
//}

@<tt>{PARAM}の値の取得と同じように文字列字句が続く限り読み込みます。
カンマ、もしくは終了の字句が来たら読み込みを終了し、その時点で読み込んだ値を返します。

==== テストを書く
ここまでの変換処理で、文字列をiCalendarで定義されているコンテンツラインへの変換の実装が終わりました。
実装が正しくできたか確認するためにテストを行います。
実際のテストを@<list>{implementation_test_content_line}に示します。
みやすさのためにテストケースは省略しています。
正しい入力に対して正しい値が取得できるかだけではなく、不正な入力を変換した場合に期待した@<code>{error}型の値を返すかも確認します。

//list[implementation_test_content_line][コンテンツラインへ変換するテストの実装][go]{
func TestContentLine(t *testing.T) {
    t.Parallel()
    tests := []struct {
        input       string
        expectValue *ContentLine
        expectError error
    }{
        {
            input: "DTSTART;VALUE=DATE:20200301",
            expectValue: &ContentLine{
                Name: "DTSTART",
                Parameters: []Parameter{
                    {
                        Name:   "VALUE",
                        Values: []string{"DATE"},
                    },
                },
                Values: []string{"20200301"},
            },
            expectError: nil,
        },
        {
            input:       "EX@MPLE:DDDD,EEEE,FFFF",
            expectValue: nil,
            expectError: fmt.Errorf("failed to get name: invalid token @"),
        },
    }

    for i, tt := range tests {
        tt := tt
        t.Run(fmt.Sprint(i), func(t *testing.T) {
            t.Parallel()
            lexer := lexer.New(tt.input)
            cl, err := ConvertContentLine(lexer)
            if err != nil {
                if tt.expectError == nil {
                    t.Fatal(err)
                }
                if err.Error() != tt.expectError.Error() {
                    t.Fatalf("unexpected error:\n\texpect: %v\n\tgot: %v",
                        tt.expectError.Error(),
                        err.Error(),
                    )
                }
            }
            if diff := cmp.Diff(tt.expectValue, cl); diff != "" {
                t.Fatal(diff)
            }
        })
    }
}
//}

=== コンテンツラインをコンポーネントに変換する
コンテンツラインのスライスからiCalendarのオブジェクトへ変換します。
この処理を構文解析と呼びます。
本来は字句解析で得られた字句から@<code>{for}文や式の抽象構文木への変換するのが構文解析です。
しかし、iCalendar形式の変換はiCalendarのコンポーネントへ変換します。

//list[input_convert_component][入力のコンテンツライン][go]{
[]*contentline.ContentLine{
    {
        Name:   "BEGIN",
        Values: []string{"VCALENDAR"},
    },
    {
        Name:   "VERSION",
        Values: []string{"2.0"},
    },
    {
        Name:   "END",
        Values: []string{"VCALENDAR"},
    },
},
//}

//list[output_convert_component][@<list>{input_convert_component}から期待する出力][go]{
&ical.Calender{
    Version: &property.Version{
        Parameter: parameter.Container{},
        Max:       types.Text("2.0"),
    },
},
//}

コンポーネントへの変換はコンテンツラインを先頭からチェックしていき次の処理をいずれかを行います。

 1. コンポーネントの開始なら新しいコンポーネントの処理に入る
 2. 終了なら、現在処理しているコンポーネントの種類とあっているか確認し結果を返す
 3. プロパティなら値を変換し、コンポーネントに設定する


==== 構文解析器の初期化
構文解析器の初期化を行います。
構文解析器の定義と初期化を@<list>{definition_initialization_parser}に示します。

//list[definition_initialization_parser][パーサの型定義と初期化][go]{
type Parser struct {
    Lines                []*contentline.ContentLine
    CurrentIndex         int
    currentComponentType component.Type
}

func NewParser(cls []*contentline.ContentLine) *Parser {
    return &Parser{
        Lines: cls,
    }
}
//}

構文解析器はすべてのコンテンツラインを持っています。
@<code>{CurrentIndex}フィールドは現在変換しているコンテンツラインを得るために使います。
@<code>{currentComponentType}フィールドは構文解析には必要ないものです。
しかし、プロパティの変換とバリデーションの際に必要になるため、保持しています。

構文解析器の初期化は受け取ったコンテンツラインのスライスを構文解析器に渡します。
@<code>{CurrentIndex}フィールドは何も指定しなければ@<code>{int}型のゼロ値である@<tt>{0}になります。
そのため、ここでは明示的に指定しません。


==== コンポーネントの開始と終了
カレンダーコンポーネントの変換を開始する処理を@<list>{parser_parse_calendar_component}に示します。

//list[parser_parse_calendar_component][カレンダーオブジェクトのパースを開始する処理][go]{
func (p *Parser) parse() (*ical.Calender, error) {
    l := p.getCurrentLine()
    switch pname := property.Name(l.Name); pname {
    case property.NameBegin:
        if len(l.Values) != 1 {
            return nil, NewInvalidValueLengthError(1, len(l.Values))
        }
        switch ct := component.Type(l.Values[0]); ct {
        case component.TypeCalendar:
            c, err := p.parseCalender()
            if err != nil {
                return nil, fmt.Errorf("parse %s: %w", ct, err)
            }
            return c, nil
        default:
            return nil, fmt.Errorf("not %s:%s, got %v",
                property.NameBegin,
                component.TypeCalendar,
                l,
            )
        }
    default:
        return nil, fmt.Errorf("not %s:%s, got %v",
            property.NameBegin,
            component.TypeCalendar,
            l,
        )
    }
}
//}

@<code>{switch}文でコンテンツラインの@<tt>{Name}フィールドを@<tt>{property}パッケージで定義した型の変数に変換します。
その変数があらかじめ定義してある変数の中からコンポーネントの開始と一致すれば、さらに@<tt>{Values}フィールドをチェックします。
@<tt>{Values}フィールドもコンポーネントの種類のための型に変換し、カレンダーを示すものと一致すれば実際にカレンダーへの変換のメソッドを呼び出します。
途中どこかで期待していない値が得られた場合、エラーを返し処理を中断します。

カレンダーコンポーネントの変換を終了する処理を@<list>{parser_parse_calendar_component}に示します。

//list[parser_end_parse_calendar_component][カレンダーオブジェクトのパースを終了する処理][go]{
func (p *Parser) parseCalender() (*ical.Calender, error) {
    p.nextLine()
    p.currentComponentType = component.TypeCalendar
    c := ical.NewCalender()

    for l := p.getCurrentLine(); l != nil; l = p.getCurrentLine() {
        switch pname := property.Name(l.Name); pname {
        // .... 他のプロパティの処理を省略しています
        case property.NameEnd:
            if !p.isEndComponent(component.TypeCalendar) {
                return nil, fmt.Errorf("Invalid END")
            }
            return c, nil
        // .... 他のプロパティやdefaultの場合の処理を省略しています
        }
     }
}
//}

終了処理は開始処理と同じようにコンテンツラインの@<tt>{Name}フィールドをプロパティの名前を表す型の変数に変換し、チェックします。
終了するコンポーネントが本当にカレンダーコンポーネントかもチェックします。
どちらも期待どおりであれば、処理を終了し変換したカレンダーコンポーネントを返します。

==== プロパティの変換
プロパティはコンポーネントのフィールドになる要素です。
プロパティはそれぞれで値の型やパラメータの型が定義されています。
そのためプロパティ毎にGoでの型を定義し、値の確認やバリデーションのメソッドを実装します。

プロパティが値のチェックをする例としてカレンダーのバージョンを表すプロパティの定義と値を設定するメソッドを@<list>{property_version_definition}に示します。

//list[property_version_definition][バージョンプロパティとその値を設定するメソッドの定義][go]{
// Version is VERSION
// https://tools.ietf.org/html/rfc5545#section-3.7.4
type Version struct {
    Parameter parameter.Container
    Min, Max  types.Text
}

func (v *Version) SetVersion(
    params parameter.Container,
    value types.Text,
) error {
    if value == "" {
        return ErrInputIsEmpty
    }
    isMatch, err := regexp.MatchString(
        `^\d+.\d+$`,
        string(value),
    )
    if err != nil {
        return err
    }
    if isMatch {
        v.Parameter = params
        v.Max = value
        return nil
    }
    isMatch, err = regexp.MatchString(
        `^\d+.\d+;\d+.\d+$`,
        string(value),
    )
    if err != nil {
        return err
    }
    if !isMatch {
        return fmt.Errorf("not required format, allow X.Y or W.X;Y.Z")
    }
    versions := strings.SplitN(string(value), ";", 2)
    return v.UpdateVersion(
        params,
        types.NewText(versions[0]),
        types.NewText(versions[1]),
    )
}

func (v *Version) UpdateVersion(
    params parameter.Container,
    min, max types.Text,
    ) error {
    a, err := semver.NewVersion(string(min))
    if err != nil {
        return fmt.Errorf("convert %s to semvar: %w", min, err)
    }
    b, err := semver.NewVersion(string(min))
    if err != nil {
        return fmt.Errorf("convert %s to semvar: %w", max, err)
    }
    if a.GreaterThan(b) {
        return fmt.Errorf("min ver %s is greater than max ver %s",
            min,
            max,
        )
    }
    v.Parameter = params
    v.Min = min
    v.Max = max
    return nil
}
//}

バージョンのプロパティの仕様はRFC 5545のSection 3.7.4@<fn>{definition_version_link}で定義されています。
このプロパティの値の型は@<tt>{TEXT}型と定義されています。
コンテンツラインから@<tt>{TEXT}型への変換については@<hd>{値の型の変換}で説明します。
しかし、バージョン番号の表現が定められています。
この表現に一致しているかを値を設定する際に詳しくチェックします。

//footnote[definition_version_link][@<href>{https://tools.ietf.org/html/rfc5545#section-3.7.4}]

@<list>{property_version_definition}のほかにも多くの種類のプロパティがあります。
すべては紹介できないため、より詳しく知りたい方はRFCを読むか、私のコードを読んでみてください。

==== 値の型の変換
プロパティの変換では、プロパティに値を渡す際に@<tt>{TEXT}型の値を渡しました。
この@<tt>{TEXT}型の変数をコンテンツラインからどのようにして取得するのかを説明します。

プロパティに設定できるデータ型はRFC 5545のSection3.3@<fn>{rfc_type_definition_link}で定義されている次の14種類です。

 1. Binary
 2. Boolean
 3. Calendar User Address
 4. Date
 5. Date-Time
 6. Duration
 7. Float
 8. Integer
 9. Period of Time
 10. Recurrence Rule
 11. Text
 12. Time
 13. URI
 14. UTC Offset

//footnote[rfc_type_definition_link][@<href>{https://tools.ietf.org/html/rfc5545#section-3.3}]

@<tt>{Float}型や@<tt>{Text}型などいくつかの型はGoで定義されている組み込み型を使うことができます。
@<tt>{Time}型や@<tt>{URI}型はGoの標準ライブラリで定義されている型を用いて代用できます。
@<tt>{Duration}型や@<tt>{Reccurence Rule}型は標準ライブラリなどでは定義されていないため、独自に定義します。

値の変換の例として@<tt>{Integer}型の値への変換を@<list>{conversion_integer}に示します。

//list[conversion_integer][整数型の定義と変換処理][go]{
// Integer is defined in https://tools.ietf.org/html/rfc5545#section-3.3.8
type Integer int64

func NewInteger(v string) (Integer, error) {
    i, err := strconv.ParseInt(v, 10, 64)
    if err != nil {
        return 0, fmt.Errorf("parse input[v] to int64: %w", err)
    }
    return Integer(i), nil
}
//}


@<tt>{Integer}型は、Goの組み込み型にある@<tt>{int64}型を利用したユーザー定義型にします。
@<tt>{int64}型をそのまま使わずに型を定義したのには理由があります。
プロパティには複数種類の型を値として保持するものがあります。
それらの型はGoの型から見ると共通でないものもあります。
その場合にユーザー定義型を作り、それらの型にプロパティのためのデータを表すインタフェースを実装します。
プロパティはこのインタフェースを値として持つようにすると、複数の値の型を意識することなく保持できます。

#@# ==== パラメータ
#@# パラメータの種類はここにある
#@# それぞれのバリデーションがあるのでそれにそってコンテンツラインから取り出したパラメータを当てはめていく
#@# parameter.Container という型を定義して、まとめて扱うことができるようにする。
#@# プロパティによっては特定のパラメータを取り出す必要があるので、mapでパラメータ名から引けるようにする。
#@# mapの要素はスライスにする。

== ファイルをパースし、オブジェクトを返す
これまで作成した機能を統合し、ライブラリとして外部へパースする機能を提供します。
次の処理を実装することで全体を通して動作させることができます。

 1. @<code>{io.Reader}から文字列を読み込む
 2. 文字列を並列にコンテンツラインへ変換する

=== io.Readerから文字列を読み込む
iCalendar形式をパースするためには対象のファイルを読み込む必要があります。
@<list>{implementation_receive_ioreader}に実装を示します。

//list[implementation_receive_ioreader][io.Readerを受け取り文字列を読み込む処理][go]{
func Parse(r io.Reader) (*ical.Calender, error) {
    return parseFromScanner(bufio.NewScanner(r))
}

func scanLines(scanner *bufio.Scanner) ([]string, error) {
    var res []string
    for scanner.Scan() {
        l := scanner.Text()
        switch {
        case strings.HasPrefix(l, " "):
            res[len(res)-1] += "\n" + strings.TrimPrefix(l, " ")
        case strings.HasPrefix(l, "\t"):
            res[len(res)-1] += "\n" + strings.TrimPrefix(l, "\t")
            continue
        default:
            res = append(res, l)
        }
    }
    if err := scanner.Err(); err != nil {
        return nil, err
    }
    return res, nil
}

func parseFromScanner(scanner *bufio.Scanner) (*ical.Calender, error) {
    lines, err := scanLines(scanner)
    if err != nil {
        return nil, err
    }
    // ... 文字列からコンテンツラインへ変換、構文解析による変換処理をおこなうが省略しています
}
//}

ファイルパスを指定して読み込む方法も考えられますが、@<tt>{io}パッケージの@<code>{io.Reader}を渡すように設計しました。
@<code>{io.Reader}を受け取るようにすると、ファイルが存在するかのチェックは利用者のコードの責任になります。
すると、パーサではより内容を変換する処理に集中できます。

さらに、@<tt>{net/http}パッケージを利用してインターネット経由で取得したiCalendar形式のデータをそのまま変換に渡すことができます。
@<list>{example_usage}に示すコードが実行できます。

//list[example_usage][iCalendarパーサを利用するサンプルコード][go]{
package main

import (
    "fmt"
    "log"
    "net/http"

    "github.com/knsh14/ical"
    "github.com/knsh14/ical/parser"
)

func main() {
    res, err := http.Get(
        "https://www.google.com/calendar/"
        +"ical/japanese__ja%40holiday.calendar.google.com/"
        +"public/basic.ics",
    )
    if err != nil {
        log.Fatal(err)
    }
    body := res.Body
    defer body.Close()
    cal, err := parser.Parse(body)
    if err != nil {
        log.Fatal(err)
    }
    for _, c := range cal.Components {
        if event, ok := c.(*ical.Event); ok {
            fmt.Println(event.Summary.Value)
        }
    }
}
//}

=== 文字列を並列にコンテンツラインへ変換する
文字列から字句解析器を生成し、コンテンツラインに変換する処理はそれぞれの行で独立した操作です。
そのため、一度文字列のスライスを生成した後は並列に処理することで、パフォーマンスを向上させることができます。
並列に処理する実装を@<list>{implementation_process_converting}に示します。

//list[implementation_process_converting][並列に文字列をコンテンツラインへ返還する処理][go]{
func parseFromScanner(scanner *bufio.Scanner) (*ical.Calender, error) {
    lines, err := scanLines(scanner)
    if err != nil {
        return nil, err
    }
    var eg errgroup.Group
    contentlines := make([]*contentline.ContentLine, len(lines))
    for i := range lines {
        i := i
        eg.Go(func() error {
            l := lexer.New(lines[i])
            cl, err := contentline.ConvertContentLine(l)
            if err != nil {
                return fmt.Errorf("convert content line in line %d: %w",
                    i,
                    err,
                )
            }
            contentlines[i] = cl
            return nil
        })
    }
    if err := eg.Wait(); err != nil {
        return nil, err
    }
    p := NewParser(contentlines)
    return p.parse()
}
//}

文字列のスライスと同じ長さのコンテンツラインのスライスを準備します。
並列に実行する処理に結果を代入するためのインデックスを渡します。
こうすると、コンテンツラインのスライスの順番がばらばらになりません。

@<tt>{errgroup}パッケージ@<fn>{errgroup_doc_link}を使うことでエラーを返す処理を並列に実行できます。
@<tt>{errgroup.Group.Wait}メソッドですべての処理が終わるまで待ちます。
どれかの処理がエラーを返すと@<code>{Wait}メソッドがエラーを返します。
もしコンテンツラインへ変換する処理のどこかでエラーが発生しても、最後にチェックできます。

//footnote[errgroup_doc_link][@<href>{https://pkg.go.dev/golang.org/x/sync/errgroup}]

== 最後に
言語処理系の実装のアイデアを利用して、iCalendar形式のファイルをパースし、Goのオブジェクトに変換するライブラリを実装しました。
言語処理系を実装するのは心理的ハードルが高いですが、まずはこのようなファイルをパースするだけのライブラリを実装することで字句解析、構文解析への理解を深めることができます。

iCalendarの仕様はあいまいに作られている部分が多く実装に苦労しましたが、便利なライブラリなのでカレンダーの処理に興味があればぜひ使ってみてください。
