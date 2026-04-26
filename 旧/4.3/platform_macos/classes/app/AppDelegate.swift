import Cocoa
import SwiftUI

@main
class AppDelegate: NSObject, NSApplicationDelegate {
    
    var window: NSWindow!
    
    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // 初始化核心模块
        FindFriendCoreBridge.shared.initializeCore()
        
        // 创建窗口
        window = NSWindow(
            contentRect: NSRect(x: 0, y: 0, width: 800, height: 600),
            styleMask: [.titled, .closable, .miniaturizable, .resizable, .fullSizeContentView],
            backing: .buffered,
            defer: false
        )
        window.center()
        window.title = "FindFriend App"
        window.setFrameAutosaveName("Main Window")
        
        // 设置内容视图
        window.contentView = NSHostingView(rootView: ContentView())
        window.makeKeyAndOrderFront(nil)
    }
    
    func applicationWillTerminate(_ aNotification: Notification) {
        // 清理资源
    }
}