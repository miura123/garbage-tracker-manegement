![Static Badge](https://img.shields.io/badge/Sony-Spresense-blue)

※ 最終版は公開準備中

# ゴミ収集者 <img src="docs/logo.png" height="25">：ゴミ管理機能について
<img src="docs/demo-ja.png" height="200"><img src="docs/measurement-ja.png" height="200">

## 説明
AIがゴミの種類を自動で判別し、対応するゴミの扉を開けてくれます。<br>
また、測距センサを用いて、ゴミの量を常に監視しています。


## 要求仕様
### ハードウェア
・Raspberry Pi 4<br>
・Spresense (拡張ボード付き)<br>
・HDRカメラボード<br>
・サーボモータ (Tower Pro) ×2<br>
・測距センサ (GP2Y0A21YK) ×2<br>

### ソフトウェア
・Raspberry Pi OS<br>
・Flask (pip installしてください)<br>
・Arduino IDE

## Raspberry Pi 4側の準備
1. garbage_flask内のapp.pyを実行
```bash
cd garbage_flask
python app.py
```

## Spresense側の準備
1. Arduino IDEを用いて、garbege_recognition内の「garbege_recognition.ino」をメインコアに書き込む。<br>
2. Arduino IDEを用いて、distance_measurment内の「distance_measurment.ino」をサブコアに書き込む。<br>
3. Spresense用のSDカードにgarbage_recognition内の「model.nnb」を書き込む。

## 配線方法
図に従い、各センサ類を繋げてください。
<img src="docs/wiring.png" height="400">