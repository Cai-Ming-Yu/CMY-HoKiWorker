package tokyo.caimingyu.hokiworker

import android.annotation.SuppressLint
import android.app.Activity
import android.app.Application
import android.content.Context
import android.content.Intent
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.os.Environment
import android.view.View
import android.widget.TextView
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.hjq.permissions.OnPermissionCallback
import com.hjq.permissions.Permission
import com.hjq.permissions.XXPermissions
import com.microsoft.appcenter.AppCenter
import com.microsoft.appcenter.analytics.Analytics
import com.microsoft.appcenter.crashes.Crashes
import com.microsoft.appcenter.distribute.Distribute
import com.molihuan.pathselector.PathSelector
import com.molihuan.pathselector.entity.FileBean
import com.molihuan.pathselector.entity.FontBean
import com.molihuan.pathselector.fragment.BasePathSelectFragment
import com.molihuan.pathselector.listener.CommonItemListener
import com.molihuan.pathselector.listener.FileItemListener
import com.molihuan.pathselector.utils.MConstants
import tokyo.caimingyu.hokiworker.library.HKLib.*
import tokyo.caimingyu.hokiworker.ui.theme.HoKiWorkerTheme
import java.io.File


open class Init : Application() {
    override fun onCreate() {
        super.onCreate()
        AppCenter.start(
            this,
            "96be348c-6655-4f04-9458-8ab4ac5f48d2",
            Analytics::class.java,
            Crashes::class.java,
            Distribute::class.java
        )
    }
}

open class Home : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        getPermissions(this, this@Home)
        setContent {
            server.body(this, this@Home)
        }
    }

    fun getPermissions(context: Context, activity: Activity) {
        when (Build.VERSION.SDK_INT) {
            in 0..29 -> {
                if (!XXPermissions.isGranted(
                        context, Permission.READ_EXTERNAL_STORAGE, Permission.WRITE_EXTERNAL_STORAGE
                    )
                ) {
                    XXPermissions.with(context).permission(Permission.READ_EXTERNAL_STORAGE)
                        .permission(Permission.WRITE_EXTERNAL_STORAGE).request(object : OnPermissionCallback {
                            override fun onGranted(permissions: MutableList<String>, allGranted: Boolean) {
                                if (!allGranted) {
                                    getPermissions(context, activity)
                                }
                            }

                            override fun onDenied(permissions: MutableList<String>, doNotAskAgain: Boolean) {
                                if (doNotAskAgain) {
                                    XXPermissions.startPermissionActivity(activity, permissions)
                                } else {
                                    getPermissions(context, activity)
                                }
                            }
                        })
                }
            }

            in 30..32 -> {
                if (!XXPermissions.isGranted(
                        context,
                        Permission.MANAGE_EXTERNAL_STORAGE,
                        Permission.READ_EXTERNAL_STORAGE,
                        Permission.WRITE_EXTERNAL_STORAGE
                    )
                ) {
                    XXPermissions.with(context).permission(Permission.MANAGE_EXTERNAL_STORAGE)
                        .permission(Permission.READ_EXTERNAL_STORAGE).permission(Permission.WRITE_EXTERNAL_STORAGE)
                        .request(object : OnPermissionCallback {
                            override fun onGranted(permissions: MutableList<String>, allGranted: Boolean) {
                                if (!allGranted) {
                                    getPermissions(context, activity)
                                }
                            }

                            override fun onDenied(permissions: MutableList<String>, doNotAskAgain: Boolean) {
                                if (doNotAskAgain) {
                                    XXPermissions.startPermissionActivity(activity, permissions)
                                } else {
                                    getPermissions(context, activity)
                                }
                            }
                        })
                }
            }

            else -> {
                if (!XXPermissions.isGranted(
                        context,
                        Permission.MANAGE_EXTERNAL_STORAGE,
                        Permission.READ_MEDIA_AUDIO,
                        Permission.READ_MEDIA_VIDEO,
                        Permission.READ_MEDIA_IMAGES
                    )
                ) {
                    XXPermissions.with(context).permission(Permission.MANAGE_EXTERNAL_STORAGE)
                        .permission(Permission.READ_MEDIA_AUDIO).permission(Permission.READ_MEDIA_VIDEO)
                        .permission(Permission.READ_MEDIA_IMAGES).request(object : OnPermissionCallback {
                            override fun onGranted(permissions: MutableList<String>, allGranted: Boolean) {
                                if (!allGranted) {
                                    getPermissions(context, activity)
                                }
                            }

                            override fun onDenied(permissions: MutableList<String>, doNotAskAgain: Boolean) {
                                if (doNotAskAgain) {
                                    XXPermissions.startPermissionActivity(activity, permissions)
                                } else {
                                    getPermissions(context, activity)
                                }
                            }
                        })
                }
            }
        }
    }
}

abstract class server {
    companion object {
        @Preview
        @SuppressLint("ComposableNaming")
        @Composable
        fun body(context: Context, activity: Activity) {
            HoKiWorkerTheme {
                Surface(
                    modifier = Modifier.fillMaxSize(), color = MaterialTheme.colorScheme.background
                ) {
                    Scaffold(modifier = Modifier.fillMaxSize(), topBar = {
                        Row(
                            modifier = Modifier.padding(top = 10.dp, start = 30.dp).height(50.dp).fillMaxWidth(),
                            horizontalArrangement = Arrangement.Start,
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Image(
                                painter = painterResource(id = R.drawable.icon),
                                contentDescription = "アイコン",
                                modifier = Modifier.height(40.dp).width(40.dp)
                            )
                            Text(
                                modifier = Modifier.padding(start = 20.dp),
                                text = stringResource(id = R.string.app_name),
                                fontSize = 24.sp,
                                fontWeight = FontWeight.Bold,
                                color = MaterialTheme.colorScheme.primary
                            )
                        }
                    }) {
                        Column(
                            modifier = Modifier.fillMaxSize()
                                .padding(top = it.calculateTopPadding() + 20.dp, start = 20.dp, end = 20.dp)
                                .verticalScroll(rememberScrollState()),
                            verticalArrangement = Arrangement.Top,
                            horizontalAlignment = Alignment.CenterHorizontally
                        ) { UI(context, activity) }
                    }
                }
            }
        }

        @Preview
        @SuppressLint("QueryPermissionsNeeded")
        @Composable
        protected fun UI(context: Context, activity: Activity) {
            val keyStr = remember { mutableStateOf("") }
            var compressionLevel by remember { mutableFloatStateOf(3f) }
            Column(
                modifier = Modifier.fillMaxWidth()
                    .background(color = MaterialTheme.colorScheme.surface, shape = RoundedCornerShape(10.dp))
                    .padding(16.dp),
                verticalArrangement = Arrangement.Top,
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                OutlinedTextField(
                    value = keyStr.value,
                    onValueChange = { keyStr.value = it },
                    label = { Text("Key(16または24または32)") },
                    modifier = Modifier.padding(8.dp)
                )
                Spacer(modifier = Modifier.height(7.dp))
                Slider(
                    value = compressionLevel, onValueChange = { newValue ->
                        compressionLevel = newValue
                    }, valueRange = 1f..19f, steps = 18, modifier = Modifier.padding(horizontal = 16.dp)
                )
                Text(
                    text = "Compression Level: ${compressionLevel.toInt()}", modifier = Modifier.padding(start = 16.dp)
                )
            }
            Box(modifier = Modifier.size(16.dp)) {}
            Column(
                modifier = Modifier.fillMaxWidth().background(
                    color = MaterialTheme.colorScheme.surface, shape = RoundedCornerShape(10.dp)
                ), verticalArrangement = Arrangement.Top, horizontalAlignment = Alignment.CenterHorizontally
            ) {
                TextButton(
                    onClick = {
                        if (keyStr.value == "") {
                            Toast.makeText(context, "キーを入力してください", Toast.LENGTH_SHORT).show()
                            return@TextButton
                        }
                        Home().getPermissions(context, activity)
                        PathSelector.build(
                            context, MConstants.BUILD_ACTIVITY
                        ).setRequestCode(1).setShowSelectStorageBtn(true).setShowTitlebarFragment(true)
                            .setShowTabbarFragment(true).setAlwaysShowHandleFragment(true).setRadio()
                            .setTitlebarMainTitle(FontBean("Choose input file"))
                            .setFileItemListener(object : FileItemListener() {
                                override fun onClick(
                                    v: View,
                                    file: FileBean,
                                    currentPath: String,
                                    pathSelectFragment: BasePathSelectFragment
                                ): Boolean {
                                    fun getOutput(): String {
                                        var index = 0
                                        val fileDir =
                                            Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS).absolutePath
                                        val fileName = "encryptedFile-${System.currentTimeMillis() / 1000}"
                                        val fileEXT = ".hk"
                                        var fileFullName = "$fileName$fileEXT"
                                        var nowFile = File("$fileDir/$fileFullName")
                                        while (nowFile.exists()) {
                                            index++
                                            fileFullName = "$fileName-$index$fileEXT"
                                            nowFile = File("$fileDir/$fileFullName")
                                        }
                                        return "$fileDir/$fileFullName"
                                    }

                                    val output = getOutput()
                                    compressEncryptFileToFile(
                                        File(output).absolutePath, compressionLevel.toInt(), keyStr.value, file.path
                                    )
                                    Toast.makeText(context, "Output file is $output", Toast.LENGTH_SHORT).show()
                                    return true
                                }
                            }).show()
                    },
                    shape = RoundedCornerShape(10.dp),
                    contentPadding = PaddingValues(0.dp),
                    modifier = Modifier.padding(top = 5.dp).height(50.dp).fillMaxWidth()
                ) {
                    Row(
                        modifier = Modifier.fillMaxSize().padding(start = 20.dp, end = 20.dp),
                        horizontalArrangement = Arrangement.Start,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(
                            modifier = Modifier.padding(start = 20.dp),
                            text = "暗号化ファイル Encrypt File",
                            fontSize = 16.sp,
                            fontWeight = FontWeight.Bold,
                            color = MaterialTheme.colorScheme.primary,
                            maxLines = 1
                        )
                        Row(
                            modifier = Modifier.fillMaxSize(),
                            horizontalArrangement = Arrangement.End,
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Image(
                                painter = painterResource(id = R.drawable.to),
                                contentDescription = null,
                                modifier = Modifier.height(16.dp).width(16.dp)
                            )
                        }
                    }
                }
                TextButton(
                    onClick = {
                        if (keyStr.value == "") {
                            Toast.makeText(context, "キーを入力してください", Toast.LENGTH_SHORT).show()
                            return@TextButton
                        }
                        Home().getPermissions(context, activity)
                        PathSelector.build(
                            context, MConstants.BUILD_ACTIVITY
                        ).setRequestCode(1).setShowSelectStorageBtn(true).setShowTitlebarFragment(true)
                            .setShowTabbarFragment(true).setAlwaysShowHandleFragment(true).setRadio()
                            .setTitlebarMainTitle(FontBean("Choose input file"))
                            .setFileItemListener(object : FileItemListener() {
                                override fun onClick(
                                    v: View,
                                    file: FileBean,
                                    currentPath: String,
                                    pathSelectFragment: BasePathSelectFragment
                                ): Boolean {
                                    fun getOutput(): String {
                                        var index = 0
                                        val fileDir =
                                            Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS).absolutePath
                                        val fileName = "decryptedFile-${System.currentTimeMillis() / 1000}"
                                        val fileEXT = ".hk"
                                        var fileFullName = "$fileName$fileEXT"
                                        var nowFile = File("$fileDir/$fileFullName")
                                        while (nowFile.exists()) {
                                            index++
                                            fileFullName = "$fileName-$index$fileEXT"
                                            nowFile = File("$fileDir/$fileFullName")
                                        }
                                        return "$fileDir/$fileFullName"
                                    }

                                    val output = getOutput()
                                    decryptDecompressFileToFile(
                                        File(output).absolutePath, keyStr.value, file.path
                                    )
                                    Toast.makeText(context, "Output file is $output", Toast.LENGTH_SHORT).show()
                                    return true
                                }
                            }).show()
                    },
                    shape = RoundedCornerShape(10.dp),
                    contentPadding = PaddingValues(0.dp),
                    modifier = Modifier.padding(top = 5.dp).height(50.dp).fillMaxWidth()
                ) {
                    Row(
                        modifier = Modifier.fillMaxSize().padding(start = 20.dp, end = 20.dp),
                        horizontalArrangement = Arrangement.Start,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(
                            modifier = Modifier.padding(start = 20.dp),
                            text = "復号化ファイル Decrypt File",
                            fontSize = 16.sp,
                            fontWeight = FontWeight.Bold,
                            color = MaterialTheme.colorScheme.primary,
                            maxLines = 1
                        )
                        Row(
                            modifier = Modifier.fillMaxSize(),
                            horizontalArrangement = Arrangement.End,
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Image(
                                painter = painterResource(id = R.drawable.to),
                                contentDescription = null,
                                modifier = Modifier.height(16.dp).width(16.dp)
                            )
                        }
                    }
                }
                Spacer(modifier = Modifier.height(3.dp))
                TextButton(
                    onClick = {
                        if (keyStr.value == "") {
                            Toast.makeText(context, "キーを入力してください", Toast.LENGTH_SHORT).show()
                            return@TextButton
                        }
                        Home().getPermissions(context, activity)
                        PathSelector.build(
                            context, MConstants.BUILD_ACTIVITY
                        ).setRequestCode(1).setShowSelectStorageBtn(true).setShowTitlebarFragment(true)
                            .setShowTabbarFragment(true).setAlwaysShowHandleFragment(true).setMaxCount(-1)
                            .setTitlebarMainTitle(FontBean("Choose input files"))
                            .setMorePopupItemListeners(object : CommonItemListener("SelectAll") {
                                override fun onClick(
                                    v: View,
                                    tv: TextView,
                                    selectedFiles: List<FileBean>,
                                    currentPath: String,
                                    pathSelectFragment: BasePathSelectFragment
                                ): Boolean {
                                    pathSelectFragment.selectAllFile(true)
                                    return false
                                }
                            }, object : CommonItemListener("DeselectAll") {
                                override fun onClick(
                                    v: View,
                                    tv: TextView,
                                    selectedFiles: List<FileBean>,
                                    currentPath: String,
                                    pathSelectFragment: BasePathSelectFragment
                                ): Boolean {
                                    pathSelectFragment.selectAllFile(false)
                                    return false
                                }
                            }).setHandleItemListeners(object : CommonItemListener("Okay") {
                                override fun onClick(
                                    v: View,
                                    tv: TextView,
                                    selectedFiles: List<FileBean>,
                                    currentPath: String,
                                    pathSelectFragment: BasePathSelectFragment
                                ): Boolean {
                                    fun getOutput(): String {
                                        var index = 0
                                        val fileDir =
                                            Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS).absolutePath
                                        val fileName = "encryptedFiles-${System.currentTimeMillis() / 1000}"
                                        val fileEXT = ".hk"
                                        var fileFullName = "$fileName$fileEXT"
                                        var nowFile = File("$fileDir/$fileFullName")
                                        while (nowFile.exists()) {
                                            index++
                                            fileFullName = "$fileName-$index$fileEXT"
                                            nowFile = File("$fileDir/$fileFullName")
                                        }
                                        return "$fileDir/$fileFullName"
                                    }

                                    val output = getOutput()
                                    var inputs = arrayOf<String>()
                                    for (fileBean in selectedFiles) {
                                        inputs += fileBean.path
                                    }
                                    compressEncryptFilesToFile(
                                        File(output).absolutePath, compressionLevel.toInt(), keyStr.value, inputs
                                    )
                                    Toast.makeText(context, "Output file is $output", Toast.LENGTH_SHORT).show()
                                    return false
                                }
                            }, object : CommonItemListener("Cancel") {
                                override fun onClick(
                                    v: View,
                                    tv: TextView,
                                    selectedFiles: List<FileBean>,
                                    currentPath: String,
                                    pathSelectFragment: BasePathSelectFragment
                                ): Boolean {
                                    pathSelectFragment.openCloseMultipleMode(false)
                                    return false
                                }
                            }).show()
                    },
                    shape = RoundedCornerShape(10.dp),
                    contentPadding = PaddingValues(0.dp),
                    modifier = Modifier.padding(top = 5.dp).height(50.dp).fillMaxWidth()
                ) {
                    Row(
                        modifier = Modifier.fillMaxSize().padding(start = 20.dp, end = 20.dp),
                        horizontalArrangement = Arrangement.Start,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(
                            modifier = Modifier.padding(start = 20.dp),
                            text = "暗号化ファイル Encrypt Files",
                            fontSize = 16.sp,
                            fontWeight = FontWeight.Bold,
                            color = MaterialTheme.colorScheme.primary,
                            maxLines = 1
                        )
                        Row(
                            modifier = Modifier.fillMaxSize(),
                            horizontalArrangement = Arrangement.End,
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Image(
                                painter = painterResource(id = R.drawable.to),
                                contentDescription = null,
                                modifier = Modifier.height(16.dp).width(16.dp)
                            )
                        }
                    }
                }
                TextButton(
                    onClick = {
                        if (keyStr.value == "") {
                            Toast.makeText(context, "キーを入力してください", Toast.LENGTH_SHORT).show()
                            return@TextButton
                        }
                        Home().getPermissions(context, activity)
                        PathSelector.build(
                            context, MConstants.BUILD_ACTIVITY
                        ).setRequestCode(1).setShowSelectStorageBtn(true).setShowTitlebarFragment(true)
                            .setShowTabbarFragment(true).setAlwaysShowHandleFragment(true).setRadio()
                            .setTitlebarMainTitle(FontBean("Choose input file"))
                            .setFileItemListener(object : FileItemListener() {
                                override fun onClick(
                                    v: View,
                                    file: FileBean,
                                    currentPath: String,
                                    pathSelectFragment: BasePathSelectFragment
                                ): Boolean {
                                    fun getOutput(): String {
                                        var index = 0
                                        val fileDir =
                                            Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS).absolutePath
                                        val fileName = "decryptedFiles-${System.currentTimeMillis() / 1000}"
                                        var fileFullName = fileName
                                        var nowFile = File("$fileDir/$fileFullName")
                                        while (nowFile.exists()) {
                                            index++
                                            fileFullName = "$fileName-$index"
                                            nowFile = File("$fileDir/$fileFullName")
                                        }
                                        return "$fileDir/$fileFullName"
                                    }

                                    val output = getOutput()
                                    decryptDecompressFilesToFile(
                                        File(output).absolutePath, keyStr.value, file.path
                                    )
                                    Toast.makeText(context, "Output dir is $output", Toast.LENGTH_SHORT).show()
                                    return true
                                }
                            }).show()
                    },
                    shape = RoundedCornerShape(10.dp),
                    contentPadding = PaddingValues(0.dp),
                    modifier = Modifier.padding(top = 5.dp).height(50.dp).fillMaxWidth()
                ) {
                    Row(
                        modifier = Modifier.fillMaxSize().padding(start = 20.dp, end = 20.dp),
                        horizontalArrangement = Arrangement.Start,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(
                            modifier = Modifier.padding(start = 20.dp),
                            text = "復号化ファイル Decrypt Files",
                            fontSize = 16.sp,
                            fontWeight = FontWeight.Bold,
                            color = MaterialTheme.colorScheme.primary,
                            maxLines = 1
                        )
                        Row(
                            modifier = Modifier.fillMaxSize(),
                            horizontalArrangement = Arrangement.End,
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Image(
                                painter = painterResource(id = R.drawable.to),
                                contentDescription = null,
                                modifier = Modifier.height(16.dp).width(16.dp)
                            )
                        }
                    }
                }
            }
            Box(modifier = Modifier.size(32.dp)) {}
            Column(
                modifier = Modifier.fillMaxWidth().background(
                    color = MaterialTheme.colorScheme.surface, shape = RoundedCornerShape(10.dp)
                ), verticalArrangement = Arrangement.Top, horizontalAlignment = Alignment.CenterHorizontally
            ) {
                TextButton(
                    onClick = {
                        context.startActivity(
                            Intent(
                                Intent.ACTION_VIEW, Uri.parse("https://github.com/Cai-Ming-Yu/CMY-HoKiWorker")
                            )
                        )
                    },
                    shape = RoundedCornerShape(10.dp),
                    contentPadding = PaddingValues(0.dp),
                    modifier = Modifier.padding(top = 5.dp).height(50.dp).fillMaxWidth()
                ) {
                    Row(
                        modifier = Modifier.fillMaxSize().padding(start = 20.dp, end = 20.dp),
                        horizontalArrangement = Arrangement.Start,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(
                            modifier = Modifier.padding(start = 20.dp),
                            text = "GitHubアドレス",
                            fontSize = 16.sp,
                            fontWeight = FontWeight.Bold,
                            color = MaterialTheme.colorScheme.primary,
                            maxLines = 1
                        )
                        Row(
                            modifier = Modifier.fillMaxSize(),
                            horizontalArrangement = Arrangement.End,
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Image(
                                painter = painterResource(id = R.drawable.to),
                                contentDescription = null,
                                modifier = Modifier.height(16.dp).width(16.dp)
                            )
                        }
                    }
                }
            }
        }
    }
}