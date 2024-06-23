# 構成
<pre>
1. 統計的手法（自己回帰型モデル）と異常検知
2. 教師なし学習による時系列データの分類
3. 決定木手法による時系列データの回帰
4. カルマンフィルタ・深層学習による回帰と異常検知
</pre>
# 実習とプログラム
<pre>
【実習1】１の解析データ（飛行旅客機の乗客数）1/all.ipynb
【実習2】ARIMAとSARIMAを比較する 1/all.ipynb
【実習3】ChangeFinder 1/cp.ipynb
【実習4】Bayes Online 1/bo.ipynb
【実習5】２の解析データ（心電図） 2/temporal/
【実習6】Kmeans（K平均法）2/temporal/kmean.ipynb
【実習7】HDBScan（階層DBScan) temporal/hdbscan.ipynb
【実習8】大阪の気温データ 2/kishou/all.ipynb
【実習9】３の解析データ （ボストンの住宅価格）3/decision-tree/all.ipynb
【実習10】決定木手法の利用 3/decision-tree/all.ipynb
【実習11】４の解析データ１（自転車利用回数） 4/lstm/LSTM.ipydb
【実習12】LSTMによる回帰 4/lstm/LSTM.ipydb
【実習13】LSTMによる異常検知 
\seminar-2023-07> dir -r | grep ipynb | grep -v Untitled | wc -l
46
</pre>

# 時系列解析の概要
<pre>
1. 記述。グラフ化、自己共分散、自己相関などの統計量。
2. モデリング。対象となる時系列データの確率的な構造を構築（または発見）する。
3. 予測。時系列データ間の相関や、過去データとの相関から将来を予測する。
</pre>

