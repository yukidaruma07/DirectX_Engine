
# 概要
* DirectXの自作ゲームエンジンです。
* このエンジンを作り始めた2026年6月時点では、DirectXのエンジンの詳細までは教えられてもらってませんでしたが（※既存のゲームエンジンによるゲーム制作のみ）、興味を持ったので、作り始めました。
* このエンジンの一部はバイブコーディングを行っていますが、コード内のコメントなどはAIに頼らず書くようにしたり、HTMLでドキュメントを書いたりし（※関連項目を参考）、理解力を上げています。

# 簡単なファイルの説明
## オブジェクト
| ファイル | 説明 |
| ---- | ---- |
| BaseObject.h | 基底クラス |
| Collder.h | コライダークラス |
| CircleCollider.cpp/h | 円のコライダー |
| FBX.cpp/h | fbxsdkによる、fbxファイルを描画するクラス |
| Image.cpp/h | 画像を描画するためのクラス |
| Textture.cpp/h | FBXなどに貼る画像ファイル（テクスチャ）クラス |
| Triangle.cpp/h | 三角形を描画するためのクラス |
| Box.cpp/h | 四角形を描画するクラス |

## シーン
| ファイル | 説明 |
| ---- | ---- |
| BaseScene.h | シーンの基底クラス |
| BootScene.cpp/h | 一番最初に呼ばれるシーンクラス |

## リソースマネージャー
| ファイル | 説明 |
| ---- | ---- |
| AudioManager.cpp/h | 音声ファイルを管理するための名前空間 |
| CSVManager.cpp/h | CSVファイルを管理するための名前空間 |

## マネージャー
| ファイル | 説明 |
| ---- | ---- |
| CameraManager.cpp/h | カメラの位置を管理するためのクラス |
| ClientSocketManager.cpp/h | ソケット通信のクライアント側のクラス（※仮）|
| ServerSocketManager.cpp/h | ソケット通信のサーバー側のクラス（※仮）|
| DirectX2DManager.cpp/h | Direct2Dを管理するのクラス |
| DirectX3DManager.cpp/h | Direct3Dを管理するためのクラス |
| InputManager.cpp/h | デバイスの入力を管理するためのクラス |
| LoggerManager.h | ログを記録するための名前空間 |
| ObjectManager.cpp/h | オブジェクトを管理するためのクラス |
| SceneManager.cpp/h | シーンを管理するためのクラス |

## シェーダー
| ファイル | 説明 |
| ---- | ---- |
| DebugPixelShader.hlsl | デバック用のピクセルシェーダ |
| DebugVertexShader.hlsl | 頂点シェーダ |
| TextturePixelShader.hlsl | テクスチャ用のピクセルシェーダ |

# ライセンス
* MIT License

# ライブラリ
* ImGUI

# 関連項目
* ドキュメント： https://github.com/programmer-2025/DirectX11_Document

# 参考
* MP3の再生： https://qiita.com/ryo_shiraishi6352/items/d4a40521bdb09c838e9d

# このエンジンによる作品

| 作品の説明 | URL |
| ---- | ---- |
| 2Dのシューティングゲーム | https://github.com/programmer-2025/2D_Shooting_DirectX |
| 3Dの迷路ゲーム | https://github.com/programmer-2025/3D_Maze_Game_DirectX |
| 3Dのタンクゲーム | https://github.com/programmer-2025/3D_Tank_Game_DirectX |
| 3Dのシューティングゲーム | https://github.com/programmer-2025/3D_Shotting_Game_DirectX |
