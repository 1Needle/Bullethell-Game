# Bullethell-Game
## 作者 : 新竹高中 洪森凱
## 此為高二自主學習之成果
### **檔案介紹**
⚠此專案相當龐大，共有2000多行，請自行斟酌觀看⚠

🔳[`bullethell.cpp`](https://github.com/1Needle/Bullethell-Game/blob/main/bullethell.cpp)\
此檔案內含有main函式，主要用於控制流程，使流程簡單易讀。

🔳[`class.h`](https://github.com/1Needle/Bullethell-Game/blob/main/class.h)\
此檔案內含有各類別的定義。

🔳[`function.cpp`](https://github.com/1Needle/Bullethell-Game/blob/main/function.cpp)\
本專案的主要檔案。\
內含有update, process_input, render等主要函式。\
此外，還有各類別的成員函式定義，以及一些常用的函式。\
由於SFML需要讀取、修改資料，因此有大量看似重複，實際上必須一行一行打的程式碼，建議可以直接跳過。\
✳若需較好的閱讀體驗，建議下載以程式碼編輯器打開，使用縮排功能便會整潔許多\
✳本人第一次寫大型專案，且經過反覆修改，程式碼相當雜亂，可能有些傷眼

### **遊戲介紹**
本遊戲以知名彈幕遊戲──東方project系列為模板進行製作，因此有相當類似的系統。\
再現了擦彈、追蹤彈、圖形彈幕等功能。\
施放技能、擊敗敵人會將敵人的子彈變為加分道具，相當華麗。\
走到畫面最上端可吸取畫面中所有道具。\
✳由於時間不足，關卡設計不完整，只有一隻boss，因此一進入遊戲便會升級為滿等狀態，讓玩家能擁有正常的boss戰體驗

⬇**遊戲下載**⬇\
[`resources.zip`](https://github.com/1Needle/Bullethell-Game/blob/main/resources.zip) & [`bullethell game.zip`](https://github.com/1Needle/Bullethell-Game/blob/main/bullethell%20game.zip)\
由於檔案過大，無法合成一個檔案上傳到github，因此分為兩個zip檔。\
遊玩時將兩個檔案都解壓縮到同一個資料夾即可點選Bullethell.exe開始遊戲。\
✳由於測試裝置不夠多，無法保證所有電腦都可順利執行此遊戲

⬇**遊玩方式**⬇\
🔳**選單操作**\
|**按鍵**|**功能**|
|:--------:|:--------:|
|方向鍵|選擇|
|Z|確認|
|Esc|取消|

🔳**遊戲中操作**\
|**按鍵**|**功能**|
|:--------:|:--------:|
|方向鍵|控制移動|
|Shift|降低移動速度|
|Z|發射子彈|
|X|施放技能|
|Esc|暫停選單|

‼**已知問題**‼\
擊殺boss時有機率因為vector溢位導致遊戲崩潰閃退。\
選單游標進行完某些動作後會消失，按一次方向鍵即會再次出現。
