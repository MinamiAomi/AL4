# MainProject

[![ReleaseBuild](https://github.com/MinamiAomi/MainProject/actions/workflows/ReleaseBuild.yml/badge.svg)](https://github.com/MinamiAomi/MainProject/actions/workflows/ReleaseBuild.yml)
[![DemoBuild](https://github.com/MinamiAomi/MainProject/actions/workflows/DemoBuild.yml/badge.svg)](https://github.com/MinamiAomi/MainProject/actions/workflows/DemoBuild.yml)
[![DebugBuild](https://github.com/MinamiAomi/MainProject/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/MinamiAomi/MainProject/actions/workflows/DebugBuild.yml)

## 開発概要
- 期間 : 2023/9 ~ 現在
- 人数 : 1人

## 開発目標
　リアルなグラフィックができるゲームエンジンを目標にしています。

## メイン機能
### パストレーシングによる描画
リアルな描画を目指すのにパストレーシングを使用しました。\
パストレーシングを使用して描画することでグローバルイルミネーションやアンビエントオクルージョンを表現できます。\
DirectXRaytracingを利用しています。\
![パストレーシング1](https://github.com/user-attachments/assets/0c2965e4-12c0-45b2-a0ce-343d346e467c)\
上は金属、下は非金属、奥に行けば行くほど荒くなります。\
![パストレーシング2](https://github.com/user-attachments/assets/abb1bde6-14ef-402f-8705-4cdc44479dea)\
パストレーシングは、半球状にランダムにレイを飛ばして色を計算します。その過程で大量のレイを飛ばす必要があります。\
サンプル数が少ないとノイズが発生してしまいます。現状ノイズの対策として平均ブラーとフレーム蓄積でデノイズをしています。

### シーンエディター
描画エンジンのデモを作成するためにシーンエディターを作成しました。\
マテリアルやモデルをエディターを用いて変更することができます。変更した結果をすぐにパストレーシングで描画します。\
![スクリーンショット 2024-11-10 235903](https://github.com/user-attachments/assets/3baf6177-c1f7-4eb1-b059-14d9b9d5c8aa)

### サブ機能
その他画面が映える機能を追加しています。
- 大気散乱\
![大気散乱](https://github.com/user-attachments/assets/11bf473c-85fc-4f8d-ac11-77eed10e54be)
- PBR　など\
![unnamed](https://github.com/user-attachments/assets/9346965e-d5d9-46df-80f0-cb37727b12a9)


## 外部ライブラリ
- Assimp
- Dear ImGui
- DirectXTex
- DirectXShaderCompiler
- nlohmann-json
