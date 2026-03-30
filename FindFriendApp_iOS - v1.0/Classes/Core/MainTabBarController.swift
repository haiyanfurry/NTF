import UIKit

class MainTabBarController: UITabBarController {
    override func viewDidLoad() {
        super.viewDidLoad()
        setupTabs()
    }

    private func setupTabs() {
        let home = UINavigationController(rootViewController: HomePage())
        home.tabBarItem = UITabBarItem(title: "首页", image: nil, tag: 0)

        let chat = UINavigationController(rootViewController: ChatPage())
        chat.tabBarItem = UITabBarItem(title: "聊天", image: nil, tag: 1)

        let mine = UINavigationController(rootViewController: MinePage())
        mine.tabBarItem = UITabBarItem(title: "我的", image: nil, tag: 2)

        viewControllers = [home, chat, mine]
    }
}
