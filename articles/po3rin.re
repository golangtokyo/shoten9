= Go+Burrows-Wheeler変換で入門する文字列解析の世界

こんにちは、pon(@po3rin)@<fn>{po3rin}です。本章ではGoによる実装を通して、
@<b>{Burrows-Wheeler変換(BWT)}@<fn>{bwt}という文字列変換アルゴリズムを紹介します。
BWTはもともと、データ圧縮などの為に開発されたアルゴリズムですが、後にその有用性が広まり、全文検索などでも利用されています。
このアルゴリズムを理解することがデータ圧縮や、全文検索などを理解する為の足がかりになるでしょう。

//footnote[po3rin][@<href>{https://twitter.com/po3rin}]
//footnote[bwt][@<href>{http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.121.6177}]

== 検証環境

この章では、次の環境で検証しています。

 * macOS 10.14.6
 * Go 1.15.1

== BWTの為の事前知識

まずはBWTの仕組みを理解するために、文字列に関する事前知識をGoの実装と共に復習しましょう。

=== 辞書式順序

@<b>{辞書式順序}とは文字列の大小の順序の決め方です。文字列T,Uを先頭から1文字ずつ比較していき、
初めて異なる文字が出てきた時に、その大小関係で順序が決まります。
Goの文字列比較では辞書式順序が採用されています(@<list>{string-order})。

//list[string-order][Goの文字列比較は辞書式順序で大小が決まる][go]{
func main() {
	if "aaab" < "aaac" {
        fmt.Println("aaab より aaacの方が大きい")
    }
}
//}

=== Suffix(接尾辞)

文字列@<code>{T}の任意の@<code>{i}番目から最後までの部分文字列を@<b>{Suffix(接尾辞)}と呼びます。
GoにはSuffixとして部分文字列が存在するかをチェックする関数である@<code>{strings.HasSuffix}@<fn>{hassuffix}があります(@<list>{hassuffix})。

//footnote[hassuffix][@<href>{https://pkg.go.dev/strings?tab=doc#HasSuffix}]

//list[hassuffix][GoにはSuffixに関する関数がある][go]{
func main() {
	fmt.Println(strings.HasSuffix("Amigo", "go")) // true
	fmt.Println(strings.HasSuffix("Amigo", "migo")) // true
	fmt.Println(strings.HasSuffix("Amigo", "")) //true
}
//}

この他にもSuffixをトリミングする@<code>{strings.TrimSuffix}@<fn>{trimsuffix}などもあります。

//footnote[trimsuffix][@<href>{https://pkg.go.dev/strings?tab=doc#TrimSuffix}]

=== Suffix Array

ここではSuffixを使ったデータ構造である@<b>{Suffix Array(接尾辞配列)}を紹介します。Suffix ArrayはBWTの構築にも利用されます。
文字列Tの全てのSuffixを辞書式順序でソートした配列です(@<img>{suffixarray})。

//image[suffixarray]["abracadarba"からSuffix Arrayを構築し、文字列パターンマッチ][scale=1]{
//}

実装は紹介しませんが、@<img>{suffixarray}からも分かるように、Suffix Arrayは文字列のパターンマッチに利用できます。
Suffixをソートしているので二分探索で探索が可能です。
GoではSuffix Arrayが標準パッケージ@<code>{index/suffixarray}@<fn>{sa}で提供されています(@<list>{suffix})。

//footnote[sa][@<href>{https://pkg.go.dev/index/suffixarray}]

//list[suffix][index/suffixarray パッケージ][go]{
import (
	"index/suffixarray"
	// ...
)

func main() {
	t := "abracadabra"
	sa := suffixarray.New([]byte(t))

	// abが出現する位置を返す。
	offsets := sa.Lookup([]byte("ab"), -1)
	fmt.Println(offsets) // [7, 0]
}
//}

Goの@<code>{index/suffixarray}パッケージ@<fn>{sa}ではSuffix Arrayの構築を@<b>{SA-IS(Suffix Array - Induced Sorting)}@<fn>{sais}というアルゴリズムを利用しています。
SA-ISによるSuffix Array構築の計算量は@<m>{O(n)}に抑えられかなり高速です。

//footnote[sais][@<href>{https://www.researchgate.net/publication/224176324_Two_Efficient_Algorithms_for_Linear_Time_Suffix_Array_Construction}]

== Burrows-Wheeler変換

それではいよいよ本題です。@<b>{Burrows-Wheeler変換(BWT)}はブロックソートとも呼ばれ、
文字列の可逆変換のアルゴリズムです。BWTはもともと、データ圧縮などの為に開発されたアルゴリズムですが、
後にその有用性が広まり、全文検索などでも利用されています。
実際にBWTはデータ圧縮プログラムである@<b>{bzip2}の内部のアルゴリズムとしても採用されており、
文字列を圧縮しやすい順番に文字を可逆変換します。

まずは、@<code>{"abracadabra"}という文字列からBWTを行う場合をみていきます。マーカーとなる文字@<code>{"$"}(他には現れない、
他の文字より小さい文字)を末尾に追加し、@<code>{"abracadabra$"}という形にした後にBWTを行うと,
@<code>{"ard$rcaaaabb"}という同じ文字が連続で並ぶ圧縮しやすい形に変換できます。
当然、BWTは可逆変換なので、@<code>{"ard$rcaaaabb"}から@<code>{"abracadabra$"}という元の文字列に戻せます(@<list>{bwt-example})。

//list[bwt-example][BWT関数での文字列変換の例][go]{
func main() {
	t := "abracadabra"
	bwt := BWT(t) // 後ほど実装
	fmt.Println(bwt) // ard$rcaaaabb

	bwtinv := BWTInverse(bwt) // 後ほど実装
	fmt.Println(bwtinv) // abracadabra
}
//}

=== BWTをGoで実装する

ではその仕組みをGoで実装しながら追っていきましょう。
BWTは文字列@<code>{T}を構成する各文字を、それに続くSuffixをキーとして辞書式順序にソートしたものです。ただし、@<code>{$}の後続のSuffixは元の文字列そのものとします。
@<code>{"abracadabra"}の文字列を入力とした例では、
@<img>{suffixarray_build}のように操作をすることでBWT後の@<code>{"ard$rcaaaabb"}という文字列を取得できます。


//image[suffixarray_build]["abracadarba$"からBWTを構築][scale=1]{
//}

これは文字列Tを一文字ずつシフトしていき、それらをソートした結果の最後の文字を結合する操作と同じです(@<img>{shift})。
こちらのほうがBWTの解説によく利用されるかもしれません。
ちなみにソートした後の最後の列を@<b>{L列}、最初の列を@<b>{F列}と呼び、F列の文字列を@<m>{Tf}とします。@<m>{Tf}に関してはBWTの復元で利用します。

//image[shift]["abracadarba$"からBWTを構築(シフト)][scale=1]{
//}

しかし、なぜこれで同じ文字が出現しやすくなるのでしょうか。各文字をそれに続くSuffixでソートするということは、
後続の文字列が似ている文字が連続で出現しやすくなるということです。
例えば、@<b>{"the"}という頻出ワードの@<b>{"t"}は後ろの@<b>{"he"}でソートされるので@<b>{"t"}は連続して出やすくなります。

それでは実際にGoでBWTを実装してみましょう。最初にSuffix Arrayを構築し、そこからBWTを構築します(@<list>{bwt-build})。

//list[bwt-build][BWT関数の実装][go]{
func BWT(t string) string {

	// Suffix Array ----------------
	t += "$"
	sa := make([]string, len(t))
	for i := 0; i < len(t); i++ {
		sa[i] = t[i:]
	}
	sort.Strings(sa)

	// BWT文字列構築 -----------------
	var result string
	for _, v := range sa {
		if len(v) < len(t) {
			result += GetChar(t, -len(v)-1)
			continue
		}
		result += GetChar(t, -1)
	}
	return result
}

// index指定で1文字だけ取得する関数
func GetChar(t string, i int) string {
	rs := []rune(t)
	if i < 0 {
		return string(rs[len(rs)+i])
	}
	return string(rs[i])
}
//}

@<code>{GetChar}関数はstringに対して1文字ずつアクセスするための関数です。
ここで、Goの@<code>{string}に対してindexアクセスする動作をもう一度復習しましょう。
Goでは文字コードをUTF-8で1byteごとに区切っています。
その為、indexアクセスではUTF-8でのbyte表現の1byteだけにアクセスしてしまいます(@<list>{byte-access})。

//list[byte-access][stringにindexアクセス][go]{
func main() {
	t:="寝たい"

	fmt.Println([]byte(t))
	// [229 175 157 227 129 159 227 129 132]

	fmt.Println(t[0])
	// 229

	fmt.Println(string(t[0]))
	// å <-寝じゃない！！
}
//}

その為、code pointを単位として文字を扱うための仕組みである@<b>{rune}を単位としてアクセスすることで
@<code>{GetChar}関数では1文字アクセスを実装しています。
実装した@<code>{BWT}関数を実際に使うと正しくBWTできていることが分かります(@<list>{bwt-build-run})。

//list[bwt-build-run][BWT関数の利用例][go]{
func main() {
	b := BWT("abracadabra")
	fmt.Println(b) // ard$rcaaaabb
}
//}

今回はSuffix Arrayを構築してBWTを構築しましたが、Suffix Arrayの構築を高速化することでBWTの構築を高速に行えます。
興味のある方はSuffix Arrayの節で紹介した@<b>{SA-IS(Suffix Array - Induced Sorting)}@<fn>{sais}を調べてみてください。

== BWT文字列の復元

続いてBWTの復元です。復元に必要なのはBWT文字列だけです。これがBWTが強力な理由です。
この節ではまず逆変換に必要な@<b>{LF-mapping}の概念について紹介します。

=== LF-mappingによるBWT復元

LF-mappingを理解するために、@<img>{shift}をもう一度確認しましょう。
まず、辞書順に並んだ各接尾辞の先頭文字をjoinした文字列を@<m>{Tf}とすると、これは@<img>{shift}におけるF列に相当します。
今回@<code>{"abracadabra$"}から得られる@<m>{Tf}は@<code>{"$aaaaabbcdrr"}です。

BWT文字列を@<m>{Tb}とすると、@<img>{shift}を見ていただくとわかる通り、
同じ行の文字、つまり@<code>{Tf[i]}と@<code>{Tb[i]}は常に隣り合って出現します。
これは1文字ずつシフトしているので当然です。そのため、$からL列 -> F列と1文字ずつ辿っていけば元の文字列が後ろから復元できます。
@<img>{inv}は@<code>{"abdec$"}を入力としてBWTを行った後の@<m>{Tf}、@<m>{Tb}から復元を行う操作を表しています。

//image[inv][TfとTbから文字列を復元][scale=1]{
//}

@<img>{inv}の例では全ての文字が一回しか出現しなかったのでL列からF列の移動がスムーズでした。
しかし、同じ文字が複数出現する場合はどちらの文字に対応しているのか一見分かりません(@<img>{inv_which})。

//image[inv_which][同じ文字が複数ある場合どっちに対応するのか一見分からない][scale=1]{
//}

しかし、@<m>{Tf}の重要な性質のおかげで、どちらの文字に対応するのかを計算できます。
@<m>{Tf}は@<b>{「1つの文字に注目した場合、Tf中にその文字が出現する順番とTbに出現する順番は同じになる」}という性質を持っています。
@<m>{Tb}のある文字が@<m>{Tf}のどの文字に紐づくかをmappingするものをLF-mappingと呼びます(@<img>{lfmapping})。

//image[lfmapping][1つの文字に注目した場合、Tf中にその文字が出現する順番とTbに出現する順番は同じになる][scale=1]{
//}

LF-mappingを行うためには、@<img>{lfmapping_eq}のように計算することでmappingできます。
配列@<m>{C[c]}はTb内で@<m>{c}より小さい文字の数です。@<m>{rank}は@<m>{Tb[:i-1]}の中の@<m>{c}の数を返します。
この二つの項でLF-mappingが可能です。

//image[lfmapping_eq][LF-mappingを行う式][scale=0.7]{
//}

例えば@<img>{lfmapping_ex}においてTbの二つ目の@<code>{b}はindexが@<code>{11}なので@<m>{LF(11)}を計算し、Tfに対応する文字の位置を特定できます。

//image[lfmapping_ex][LF-mappingの使用例][scale=1]{
//}

これで同じ文字が複数回出現しても@<m>{Tf}と@<m>{Tb}の文字の対応がわかるようになりました。
ここまでの知識でLF-mappingを使ってBWTから元の文字列を復元できます。

=== BWT復元のGo実装

それではLF-mappingを使ったBWT復元をGoで実装します。まず@<m>{C}を構築し、
そこに出現文字を回数分足していくことで LF-mappingを生成して、文字列復元を行います(@<list>{bwt-inverse})。

//list[bwt-inverse][BWT文字列の復元][go]{
func BWTInverse(t string) string {

	// Cの構築 ------------
	C := make(map[rune]int)
	for _, c := range t {
		v, ok := C[c]
		if !ok {
			C[c] = 1
			continue
		}
		C[c] = v + 1
	}

	sig := deduplicate(strings.Split(t, "")) // 後で実装

	var sum int
	for _, c := range sig {
		r := []rune(c)[0]
		cur := C[r]
		C[r] = sum
		sum = sum + cur
	}

	// LF-mapping ----------

	lf := make([]int, len(r))
	for i, c := range t {
		lf[C[c]] = i
		C[c] = C[c] + 1
	}

	// inverse -------------

	r := []rune(t)
	p := strings.Index(t, "$")
	lf := make([]int, utf8.RuneCountInString(t))
	for i := range t {
		p = lf[p]
		result[i] = r[p]
	}

	// 最後の$を外して返す
	return string(result[:len(result)-1])
}
//}

@<code>{utf8.RuneCountInString}は@<code>{string}型を受け取り@<code>{rune}の数を返します。純粋に文字数を取得したい場合に便利です。
@<code>{deduplicate}関数は出現文字だけのスライスを取得する為の重複排除関数です(@<list>{deduplicate})。
この実装はGo公式の@<b>{SliceTricks}@<fn>{slicetricks}の@<b>{In-place deduplicate}を参考にしています。
SliceTricksではその他にもGopherが知るべきテクニックが満載なので是非読んでみてください。

//footnote[slicetricks][@<href>{https://github.com/golang/go/wiki/SliceTricks#in-place-deduplicate-comparable}]

//list[deduplicate][重複文字を排除して返す関数][go]{
func deduplicate(s []string) []string {
	var result []string
	sort.Strings(s)
	j := 0
	for i := 1; i < len(s); i++ {
		if s[j] == s[i] {
			continue
		}
		j++
		s[j] = s[i]
	}
	result = s[:j+1]
	return result
}
//}


実際に@<code>{BWTInverse}を使ってみるとBWTから元の文字列を復元できていることが分かります(@<list>{bwt-example2})。

//list[bwt-example2][BWT関数での文字列変換の例][go]{
func main() {
	t := "abracadabra"
	bwt := BWT(t)
	fmt.Println(bwt) // ard$rcaaaabb

	bwtinv := BWTInverse(bwt)
	fmt.Println(bwtinv) // abracadabra
}
//}

おめでとうございます。これでGoによるBWT実装ができました。

== まとめ

今回はGoでBurrows-Wheeler変換について紹介しました。
Burrows-Wheeler変換は色んな分野に応用され、奥が深いアルゴリズムなので、是非みなさんも深ぼってみて下さい。
