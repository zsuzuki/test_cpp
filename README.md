# test_cpp
c++14以降で使える機能を使って作ったテストプログラム集。

### atomic_queue.h
atomicを使った、スレッドセーフなキュー。
シンプルにするために、ポインタのみ扱う。

### worker.h
マルチスレッドで、ラムダを実行させる。
いわゆるジョブ。
単に登録したものを実行するだけ。

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
zlibを使うテスト。

### size.cpp
構造体のサイズを表示するだけ。

### toml.cpp
tomlで定義した構造をC言語のstructにしてダンプする。
[cpptoml](https://github.com/skystrife/cpptoml)を使用しています。

### number_shuffle.cpp
uint32_tの数値に、簡単なエンコードを施して、uint64_tにして返す。難読化テスト。

### string_encode.cpp
最長12文字までの文字列を、uint64_tに変換する。文字種が限られるので、"[]"などは"()"に変換する。
大文字も使えない(勝手に小文字にする)。
