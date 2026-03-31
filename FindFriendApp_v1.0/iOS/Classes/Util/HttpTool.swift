import Foundation

class HttpTool {
    // 缓存管理器
    private static let cache = URLCache(memoryCapacity: 10 * 1024 * 1024, diskCapacity: 50 * 1024 * 1024, diskPath: "http_cache")
    
    static func request(url: String, params: [String: Any]?, completion: @escaping (Any?) -> Void) {
        guard let url = URL(string: url) else {
            completion(nil)
            return
        }
        
        // 检查缓存
        let request = URLRequest(url: url)
        if let cachedResponse = cache.cachedResponse(for: request) {
            if let data = cachedResponse.data {
                completion(try? JSONSerialization.jsonObject(with: data, options: []))
                return
            }
        }
        
        // 实际网络请求
        let task = URLSession.shared.dataTask(with: request) { data, response, error in
            if let error = error {
                print("网络请求错误: \(error)")
                completion(nil)
                return
            }
            
            if let data = data {
                // 缓存响应
                if let response = response as? HTTPURLResponse {
                    let cachedResponse = CachedURLResponse(response: response, data: data)
                    cache.storeCachedResponse(cachedResponse, for: request)
                }
                
                completion(try? JSONSerialization.jsonObject(with: data, options: []))
            } else {
                completion(nil)
            }
        }
        
        task.resume()
    }
    
    // 清除缓存
    static func clearCache() {
        cache.removeAllCachedResponses()
    }
}
