lispインタプリターです，
ガベージコレクションは動きません．
プログラム終了時にアロケートしたオブジェクトの
領域が開放されるようになっています。
以下のように実行します。

lisp.exe <src name>

プログラムは基本てきにlispで書きます。
例として以下のように実行します
lisp.exe test.lisp

以下の組み込み関数がつかえます。
eval
lambda
defun
car
cdr
cons
list
progn
setq
let
print
quote
if
cond
while
eq
cos
sin
tan
acos
asin
atan
atan2
abs
pow
exp
sqrt
log
log10
to-deg
to-rad

以下の演算子が使えます
+
-
/
*

以下の条件演算子が使えます
!
=
<
<=
>
>=

プログラム例

(defun fact (n)
  (if (= n 1)
      1
    (* n (fact (- n 1)))))

(fact 8)

階乗をもとめるプログラムです。
このばあいfactに8をわたして8の階乗を計算します。




