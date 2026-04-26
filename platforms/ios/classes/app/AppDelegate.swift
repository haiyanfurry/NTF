import UIKit

@main
class AppDelegate: UIResponder, UIApplicationDelegate {
    var window: UIWindow?

    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
        // 初始化核心系统
        if !FindFriendCoreBridge.shared.initCore() {
            print("Failed to initialize core system")
        } else {
            print("Core system initialized successfully")
        }
        
        // 优化启动时间：延迟加载非必要资源
        performSelector(inBackground: #selector(loadNonEssentialResources), with: nil)
        
        let window = UIWindow(frame: UIScreen.main.bounds)
        self.window = window
        window.backgroundColor = .white
        window.rootViewController = MainTabBarController()
        window.makeKeyAndVisible()
        
        // 设置崩溃统计
        setupCrashReporting()
        
        return true
    }
    
    func applicationWillTerminate(_ application: UIApplication) {
        // 销毁核心系统
        FindFriendCoreBridge.shared.destroyCore()
        print("Core system destroyed")
    }
    
    @objc private func loadNonEssentialResources() {
        // 加载非必要资源，如图片缓存、字体等
        // 这里可以添加具体的资源加载代码
    }
    
    private func setupCrashReporting() {
        // 设置崩溃统计，实际项目中可以使用第三方库如Firebase Crashlytics
        // 这里添加基础的崩溃处理
        NSSetUncaughtExceptionHandler { exception in
            print("崩溃原因: \(exception)")
            print("崩溃堆栈: \(exception.callStackSymbols)")
        }
    }
}
