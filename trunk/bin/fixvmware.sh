#!/bin/bash

cd    "/Library/Application Support/VMware Fusion/isoimages"
mkdir original
mv    darwin.iso tools-key.pub *.sig original

sed "s/ServerVersion.plist/SystemVersion.plist/g" < original/darwin.iso > darwin.iso

openssl genrsa -out tools-priv.pem 2048
openssl rsa -in tools-priv.pem -pubout -out tools-key.pub
openssl dgst -sha1 -sign tools-priv.pem < darwin.iso > darwin.iso.sig
for A in *.iso ; do openssl dgst -sha1 -sign tools-priv.pem < $A > $A.sig ; done

