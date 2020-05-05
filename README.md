# SVM practice
Graph Cutを用いたセグメンテーションの練習と，B4輪講の引継ぎ．

## Requirement
- Visual Studio 2019  
    - Nugetパッケージの管理から以下のものをインストール
        - boostfilesytem vc142  
        - Eigen  
        - itk_static_vc142-x64-release 
    - ソリューション構成，ソリューションプラットフォームはそれぞれ，Release,  x64とする  

## Usage
1. GraphCutSegmentation.batを実行
2. evaluation.batを実行  
パスの設定は相対パスを使用しているので，フォルダ構成に注意(.iniファイル参照)．

