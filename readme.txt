Chroma Shift filter for AviUtl ver.0.0.0

このプラグインはまるも製作所の色タイミング補正プラグイン
(http://www.marumo.ne.jp/auf/#yctiming)
を書き直したものです。
AviUtlで、色差信号を水平・垂直方向に 1/4 ピクセル単位でずらします。


必要なもの：
- AviUtl 0.99k以降
- Windows Vista sp2以降
- SSE2が使えるCPU
- Microsoft VisualC++ Redistributable Package 2015.


使用方法：
設定値は 1/4 ピクセル単位です。水平/垂直方向に最大32ピクセルまでずらせます。
画像端処理はパディングのみです(オリジナルの"端の色で塗りつぶす"と同じ)。
syncにチェックを入れるとCbとCrが同期します。


ライセンス：
The MIT License

Copyright (c) 2016 OKA Motofumi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


