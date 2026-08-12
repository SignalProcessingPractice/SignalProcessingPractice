# Signal Processing Practice
## ディレクトリ構成 (予定)

```
├ app/
├ llb/
└ cmake/
```

## Dependencies
- Qt6

### Windows 版ビルド

#### ビルド要件

Windows 版の本アプリをビルドするには、事前に以下の環境変数を設定してください。

| 環境変数                | 説明                                      | 設定例                        |
| ------------------- | --------------------------------------- | -------------------------- |
| `QT_ROOT_DIR`           | Qt のインストールディレクトリ                        | `C:\Qt\6.11.0\mingw_64`    |
| `QT_TOOLCHAIN_ROOT` | Qt と ABI 互換の MinGW ツールチェーンのインストールディレクトリ | `C:\Qt\Tools\mingw1310_64` |

#### インストール

`app-win` プリセットの `CMAKE_INSTALL_PREFIX` に従う。
