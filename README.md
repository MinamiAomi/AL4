# MainProject

## 開発概要
- 期間 : 2023/9 ~ 現在
- 人数 : 1人

## 開発目標
　リアルなグラフィックができるゲームエンジンを目標にしています。

## メイン機能
### パストレーシングによる描画
パストレーシングを使用して描画することでグローバルイルミネーションやアンビエントオクルージョンを表現できます。\
![パストレーシング1](https://github.com/user-attachments/assets/0c2965e4-12c0-45b2-a0ce-343d346e467c)\
上は金属、下は非金属、奥に行けば行くほど荒くなります。\
![パストレーシング2](https://github.com/user-attachments/assets/abb1bde6-14ef-402f-8705-4cdc44479dea)\
現状ノイズの対策として平均ブラーとフレーム蓄積でデノイズをしています。

### シーンエディター


### サブ機能
- 大気散乱
- ブルーム
- PBR　など\
![大気散乱](https://github.com/user-attachments/assets/11bf473c-85fc-4f8d-ac11-77eed10e54be)

## 外部ライブラリ
- Assimp
- Dear ImGui
- DirectXTex
- DirectXShaderCompiler
- nlohmann-json
