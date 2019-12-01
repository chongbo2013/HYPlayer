package com.cw.hyplayer.viewer

import android.content.Context
import android.util.AttributeSet
import android.util.Log
import android.view.*
import com.almeros.android.multitouch.MoveGestureDetector
import com.almeros.android.multitouch.RotateGestureDetector
import com.almeros.android.multitouch.ShoveGestureDetector

class NativeObjView : SurfaceView, SurfaceHolder.Callback, Choreographer.FrameCallback {

    constructor(context: Context?) : super(context)
    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs)
    constructor(context: Context?, attrs: AttributeSet?, defStyleAttr: Int) : super(
        context,
        attrs,
        defStyleAttr
    )

    private var active = false

    private var mScaleDetector: ScaleGestureDetector? = null
    private var mRotateDetector: RotateGestureDetector? = null
    private var mMoveDetector: MoveGestureDetector? = null
    private var mShoveDetector: ShoveGestureDetector? = null

    private val screenWidth = resources.displayMetrics.widthPixels
    private val screenHeight = resources.displayMetrics.heightPixels

    init {
        holder.addCallback(this)
        mScaleDetector = ScaleGestureDetector(context.applicationContext, ScaleListener(this))
        mRotateDetector = RotateGestureDetector(context.applicationContext, RotateListener(this))
        mMoveDetector = MoveGestureDetector(context.applicationContext, MoveListener(this))
        mShoveDetector = ShoveGestureDetector(context.applicationContext, ShoveListener(this))
    }

    override fun onTouchEvent(event: MotionEvent?): Boolean {
        mScaleDetector?.onTouchEvent(event)
        mRotateDetector?.onTouchEvent(event)
        mMoveDetector?.onTouchEvent(event)
        mShoveDetector?.onTouchEvent(event)
        return true
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        nativeObjViewCreated(holder!!.surface)
        active = true
        Choreographer.getInstance().postFrameCallback(this)
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        nativeObjViewChanged(width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        active = false
        Choreographer.getInstance().removeFrameCallback(this)
        nativeObjViewDestroyed()
    }

    override fun doFrame(frameTimeNanos: Long) {
        if (active) {
            nativeObjViewDoFrame()
            Choreographer.getInstance().postFrameCallback(this)
        }
    }

    private inner class ScaleListener(val gestureSurfaceView: NativeObjView) :
        ScaleGestureDetector.SimpleOnScaleGestureListener() {
        override fun onScale(detector: ScaleGestureDetector): Boolean {
//            Log.d(TAG, "scale ${detector.scaleFactor}")
            return true
        }
    }

    private inner class RotateListener(val gestureSurfaceView: NativeObjView) :
        RotateGestureDetector.SimpleOnRotateGestureListener() {
        override fun onRotate(detector: RotateGestureDetector): Boolean {
//            Log.d(TAG, "rotate ${detector.rotationDegreesDelta}")
            return false
        }
    }

    private inner class MoveListener(val gestureSurfaceView: NativeObjView) :
        MoveGestureDetector.SimpleOnMoveGestureListener() {
        override fun onMove(detector: MoveGestureDetector): Boolean {
//            Log.d(TAG, "move ${detector.focusDelta.x} ${detector.focusDelta.y}")
            return false
        }
    }

    private inner class ShoveListener(val gestureSurfaceView: NativeObjView) :
        ShoveGestureDetector.SimpleOnShoveGestureListener() {
        override fun onShove(detector: ShoveGestureDetector): Boolean {
//            Log.d(TAG, "shove ${detector.shovePixelsDelta}")
            return false
        }
    }

    private external fun nativeObjViewCreated(surface: Surface)
    private external fun nativeObjViewChanged(width: Int, height: Int)
    private external fun nativeObjViewDestroyed()
    private external fun nativeObjViewDoFrame()

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
        private const val TAG = "NativeObjView"
    }

}