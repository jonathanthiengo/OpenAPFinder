import hashlib
import binascii
 

   
def generate_hash_256(data):
   SEED = 8
   hash = hashlib.sha256(data).digest()
   hash = hashlib.sha256(hash).digest()
   doublehashed = binascii.hexlify(hash).decode('utf8')
   return doublehashed.upper()
   
   

