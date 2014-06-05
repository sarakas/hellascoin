from jsonrpc import ServiceProxy

pwd = raw_input("Enter wallet passphrase: ")
access.walletpassphrase(pwd, 60)
