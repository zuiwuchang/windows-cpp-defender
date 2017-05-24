# windows-cpp-defender
windows 下的一個 服務包裝軟體 同時是一個看門狗 軟體


# why
在中華淪陷區的一個公司混飯吃 company的服務器基本都在 windows平臺下 然windows平臺的服務軟體 需要按照一定規格 調用系統api 對於使用 如 golang 這樣方便的 東西 去調用 windows api 會各種問題 最明顯的在於 無法 在 linux的舒適環境下開發後 直接交叉編譯到 windows 而且 所有軟體都會被 ms 綁架(按照 windows服務 要求的 規格 工作)

孤希望 任何一個 可執行檔A 都可以 作爲 服務 最方便的方式 莫過於 直接寫個服務 軟體B 之後由B 其 啓動 關閉A

有用的 服務功能 通常有複雜的邏輯 從而 提高了 崩潰的 可能 故單獨啓動一個 邏輯相對簡單(所以不容易崩潰) 的軟體 去監視 服務功能 在啓崩潰後 重新啓動是必要的 defender 亦提高此功能

# how
編輯 my.json 在其中 配置 要包裝的 服務 信息

service -i 安裝服務

service -u 卸載服務

service -s 以windows服務方式運行

service  以普通進程 運行

# my.json
<pre>
{
	//服務名稱
	"Name":"king-defender-test",
	//服務 顯示名稱
	"Show":"king defender 測試服務",
	//服務 描述
	"Description":"用於 測試 king defender 組件是否正常工作的 服務",
	//啓動方式是否爲 自動
	"Auto":true,
	//被守衛進程 路徑
	"Bin":"simulator.exe",
	//被守衛進程 執行參數
	"Params":""
}</pre>

# 結束被守衛進程
service 不會粗暴的 強制結束 被守衛的進程 而是通知被守衛的進程 應該結束 並調用 被守衛的進程自己的 結束代碼 退出 進程

然而 你不需要 爲 被守衛的進程 添加 任何代碼(這意味着 通常 你可以將你任意一個進程交給 service 而不會被 service 的架構綁架 並且可以優雅的結束 進程（而不是 被 ExitProcess 或 TerminateProcess) service 會 負責 將hook.dll 注入到你的進程中 所有需要的 額外代碼 hook.dll 會負責處理

service 會創建一個 命名的 event 當需要 通知 進程結束時 會 將event 設置爲 激活的 以通知 hook.dll 進程應該被關閉了(比如os關機 時進程應該 儘快的優雅退出 以免 os強制 關閉 引起的 數據錯誤)

hook.dll 被注入後 會 hook windows的 SetConsoleCtrlHandler 函數 當被守衛的進程使用 SetConsoleCtrlHandler 會獲取到 註冊的 ctrl事件 處理函數地址 同時 hook.dll 會一直等待 service的 退出通知(通過命名 event) 在接到 通知後 如果 已經獲取到了 ctrl事件 的 handler 函數 則爲以 CTRL_C_EVENT 調用此函數 此時 被守衛的進程邊可以執行 自己的 ctrl+c 事件中的 結束代碼

# SetConsoleCtrlHandler
SetConsoleCtrlHandler 通常在windows console程序中 註冊一個 處理 函數 以接收 ctrl + c/用戶登出/os關閉等 事件 任何優雅退出的好的 console 軟體都應該 使用 SetConsoleCtrlHandler

hook.dll會 在 需要退出 被守衛的進程 時 調用 SetConsoleCtrlHandler 註冊的 handler

如果你的 軟體沒有 使用 SetConsoleCtrlHandler 意味者 你的進程即時被 強制 結束也沒關係 此時 hook.dll 會直接 使用 ExitProcess(1) 結束進程

# vs-service
vs-service 是 service 的源碼 負責  與windows 的scm 通信 以及 監視 被守衛的進程 並在合適的時候通知 其退出

# vs-hook
vs-hook 是 hook.dll 的源碼

