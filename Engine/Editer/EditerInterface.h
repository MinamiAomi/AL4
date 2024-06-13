#pragma once

// エディターで選択可能
class SelectableInEditer {
public:
    virtual ~SelectableInEditer() {}
    // インスペクタービューで描画する
    virtual void RenderInInspectorView() = 0;
};