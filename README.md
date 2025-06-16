# マルチスレッドレンダリング

## 概要

このプロジェクトは、**DirectX 11** と **DirectXTK** を用いた、
**マルチスレッドによる描画処理** を備えたリアルタイムレンダリングのサンプルです。

スレッドごとに描画処理を分散させることで、CPUリソースの有効活用と描画負荷の軽減を目指します。

## 特徴

- 複数スレッドによる描画処理（`ID3D11DeviceContext::FinishCommandList` を使用）
- 各スレッドでコマンドリストを構築し、メインスレッドで統合描画
- Dear ImGui によるデバッグ表示

## 動作環境
・OS：Windows 10 / 11  
・GPU : Direct3D 11対応グラフィックカード  
・開発環境：Visual Studio 2022  
・ビルド対象：x64  

## 使用ライブラリ
・Direct3D 11  
・DirectXTK  
・Windows SDK  
・Dear ImGui  

## 現在の課題

- 🔧 **モデルのちらつくバグを検証中です。**  
  複数スレッドによる描画順序やリソース競合が原因の可能性があるため、
  `Deferred Context` の扱いや同期処理（`ID3D11DeviceContext::ExecuteCommandList`）を中心に調査を進めています。
