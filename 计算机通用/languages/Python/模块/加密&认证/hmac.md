# Hmac

Hmac 包/模块是用来是实现HMAC(Hash-based Message Authentication Code)的


主要通过**密钥+消息+hash算法生成一段验证码，用来验证消息没有被篡改，以及发送方是否持有正确的密钥**



```python
import hmac
import hashlib

key = b"secret_key"
msg = b"hello world"

# 生成 HMAC-SHA256，转成十六进制
signature = hmac.new(key, msg, hashlib.sha256).hexdigest()
print(signature)

# 验证
received_sig = signature
expected_sig = hmac.new(key, msg, hashlib.sha256).hexdigest()

if hmac.compare_digest(expected_sig, received_sig):
    print("验证通过")
else:
    print("验证失败")
```