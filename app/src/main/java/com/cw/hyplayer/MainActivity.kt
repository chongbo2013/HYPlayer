package com.cw.hyplayer

import android.Manifest
import android.content.pm.PackageManager
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Toast
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.cw.hyplayer.audio.HYAudioPlayer
import com.cw.hyplayer.audio.MediaSource
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    private var audioPlayer: HYAudioPlayer? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        if (Build.VERSION.SDK_INT >= 21 && ContextCompat.checkSelfPermission(
                this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            ActivityCompat.requestPermissions(
                this,
                arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE),
                PERMISSION_CODE
            )
        }

        create.setOnClickListener {
            if (audioPlayer == null) {
                val mediaSource = MediaSource("/sdcard/test.mp3")
//                val mediaSource = MediaSource("/sdcard/trailer111.mp4")
                audioPlayer = HYAudioPlayer(mediaSource)
            }
        }

        release.setOnClickListener {
            audioPlayer?.release()
            audioPlayer = null
        }

        start.setOnClickListener {
            audioPlayer?.start()
        }
        pause.setOnClickListener {
            audioPlayer?.pause()
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        if (requestCode == PERMISSION_CODE && grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
        } else {
            Toast.makeText(this, "must need write external storage permission!", Toast.LENGTH_LONG)
                .show()
        }
    }

    companion object {
        private const val PERMISSION_CODE = 10001
    }
}
