# MainProject

## 開発概要
- 期間 : 2023/9 ~ 現在
- 人数 : 1人

## 開発目標
　リアルなグラフィックができるゲームエンジンを目標にしています。

## メイン機能
- パストレーシングによる描画
- シーンエディター
![demo1](https://github.com/user-attachments/assets/8f89289e-c08d-4995-9b4d-6d66262a8ff3)

### サブ機能
- 大気散乱
- ブルーム
- PBR　など\
![スクリーンショット 2024-10-30 174907](https://github.com/user-attachments/assets/1b2e08c8-ff69-495f-8bbd-84d484427570)

## 階層
- Engine エンジン部分
  - Audio
  - Collision
  - Debug
  - Editer
  - Externals
  - File
  - GameObject
  - Graphics
    - Core
    - Raytracing 
    - Shader
  - Input
  - Math
  - Scene
  - Utility
- Test デモ部分
  - Resources
## 外部ライブラリ
- Assimp
- Dear ImGui
- DirectXTex
- DirectXShaderCompiler
- nlohmann-json
