import UIKit

class BasePage: UIViewController {
    override func viewDidLoad() {
        super.viewDidLoad()
        view.backgroundColor = .white
        setupPlaceholder()
        
        // 注册内存警告通知
        NotificationCenter.default.addObserver(self, selector: #selector(handleMemoryWarning), name: UIApplication.didReceiveMemoryWarningNotification, object: nil)
    }
    
    deinit {
        // 移除通知观察者
        NotificationCenter.default.removeObserver(self)
        print("\(type(of: self)) 已释放")
    }

    private func setupPlaceholder() {
        let label = UILabel()
        label.text = "\(type(of: self))\n框架已就绪"
        label.numberOfLines = 0
        label.textAlignment = .center
        label.translatesAutoresizingMaskIntoConstraints = false
        view.addSubview(label)

        NSLayoutConstraint.activate([
            label.centerXAnchor.constraint(equalTo: view.centerXAnchor),
            label.centerYAnchor.constraint(equalTo: view.centerYAnchor)
        ])
    }
    
    @objc private func handleMemoryWarning() {
        // 处理内存警告，释放不必要的资源
        print("\(type(of: self)) 收到内存警告")
        // 这里可以添加具体的内存释放代码
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        // 视图即将出现时的处理
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)
        // 视图消失时的处理，释放不必要的资源
    }
}
