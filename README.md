# test_cpp
c++14以降で使える機能を使って作ったテストプログラム集。

### atomic_queue.h
atomicを使った、スレッドセーフなキュー。
シンプルにするために、ポインタのみ扱う。
-> 整数型など、プリミティブ型は扱えるようにした。

### worker.h
マルチスレッドで、ラムダを実行させる。
いわゆるジョブ。
単に登録したものを実行するだけ。

### zlib.hpp
zlibを使って、.gz形式の圧縮・展開を行う。

### queue.cpp
atomic_queue.hのテストコード。

### thread_test.cpp
worker.hのテストコード。

### input_step.cpp
ゲームなどで使用する、ボタン入力のテスト。

### function_call.cpp
クラスの特定メソッドを登録して実行させるテスト。
ラムダに比べて、ポインタ登録するだけなので高速。

### memcached.cpp
memcachedの使用テストコード。

### zlib_test.cpp
zlib.hppを使うテスト。サンプルはファイルを.gzで圧縮・展開する。

### size.cpp
構造体のサイズを表示するだけ。

### toml.cpp
tomlで定義した構造をC言語のstructにしてダンプする。
[cpptoml](https://github.com/skystrife/cpptoml)を使用しています。

### number_shuffle.cpp/number_encode.h
uint32_tの数値に、簡単なエンコードを施して、uint64_tにして返す。難読化テスト。

### string_encode.cpp/string_encode.h
最長12文字までの文字列を、uint64_tに変換する。文字種が限られるので、"[]"などは"()"に変換する。
大文字も使えない(勝手に小文字にする)。

### charcount.cpp
ファイルに存在する文字の数をカウントする。
UTF-8等、ASCII以外の文字は考慮しない。

### make_random.cpp
ビット数(マスク)と要素数を指定して、ランダムテーブルを生成する。

### dynamic_cast.cpp/context.h
アプリケーションのコンテキスト情報の格納などに使えるような、任意の型のshared_ptrを保持するクラスのテスト。

### game_test.cpp/game_struct.hpp
ゲーム的な構造体をtomlで生成して、それを使って、ランダムで簡単な戦闘を行わせる。
