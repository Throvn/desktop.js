import * as os from "os"
import * as minnet from "./lib/minnet-quickjs/minnet.so"
print(os.getcwd())
console.log("Hey, this is a test from js!")


function getDownloadCount() {
    const res = minnet.fetch("https://api.github.com/repos/khanhas/spicetify-cli/releases")
    const dl_count = res.json().reduce((total, tag) => {
        return total += tag.assets.reduce((tag_total, asset) => {
            return tag_total += asset.download_count
        }, 0)
    }, 0)
    print("Fetch:", res.url)
    print("STATUS: ", res.status, "OK: ", res.ok, "TYPE: ", res.type)
    print("RESULT: ", dl_count)
    return `${dl_count}`
}
getDownloadCount()

print("Request done")