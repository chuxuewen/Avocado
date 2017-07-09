/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015, Louis.chu
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Louis.chu nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Louis.chu BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ***** END LICENSE BLOCK ***** */

package com.avocado;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Typeface;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.BatteryManager;
import android.os.Build;
import android.os.Debug;
import android.telephony.TelephonyManager;
import android.util.DisplayMetrics;
import java.util.List;
import java.util.Locale;
import android.app.ActivityManager;
import android.app.NativeActivity;
import android.util.Log;
import android.view.WindowManager;

public class Avocado {
  private static String TAG = "Avocado";
  private static NativeActivity activity = null;
  private static BroadcastReceiver receiver = null;
  private static int battery_status = BatteryManager.BATTERY_STATUS_UNKNOWN;
  private static boolean is_ac_power_connected = false;

	public static void initializ(NativeActivity act) {
		Avocado.activity = act;
    keep_screen(true);

    receiver = new BroadcastReceiver() {
      public void onReceive(Context context, Intent intent) {
        if ( intent.getAction() == Intent.ACTION_POWER_CONNECTED ) {
          is_ac_power_connected = true;
        } else if ( intent.getAction() == Intent.ACTION_POWER_DISCONNECTED ) {
          is_ac_power_connected = false;
        } else {
          battery_status = intent.getIntExtra("status", BatteryManager.BATTERY_STATUS_UNKNOWN);
        }
      }
    };
    activity.registerReceiver(receiver, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
    activity.registerReceiver(receiver, new IntentFilter(Intent.ACTION_POWER_CONNECTED));
    activity.registerReceiver(receiver, new IntentFilter(Intent.ACTION_POWER_DISCONNECTED));
  }

  public static void keep_screen(boolean value) {
    if ( value ) {
      activity.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    } else {
      activity.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }
  }

  public static float display_scale() {
    DisplayMetrics dm = new DisplayMetrics();
    activity.getWindowManager().getDefaultDisplay().getMetrics(dm);
    return dm.scaledDensity;
  }

  public static String package_code_path() {
    return activity.getPackageCodePath();
  }

  public static String files_dir_path() {
    return activity.getFilesDir().getPath();
  }

  public static String cache_dir_path() {
    return activity.getCacheDir().getPath();
  }

  public static String default_typeface() {
    return "";
  }

  public static String version() {
    return Build.VERSION.RELEASE;
  }
  
  public static String brand() {
    return Build.BRAND;
  }
  
  public static String subsystem() {
    return Build.MODEL;
  }
  
  public static int network_status() {
    ConnectivityManager m = (ConnectivityManager)
            activity.getSystemService(Context.CONNECTIVITY_SERVICE);
    NetworkInfo info = m.getActiveNetworkInfo();

    if ( info != null && info.isAvailable() ) {
      int type = info.getType();
      if (type == ConnectivityManager.TYPE_WIFI) {
        return 2;
      } else if (type == ConnectivityManager.TYPE_MOBILE) {

        int sub_type = info.getSubtype();
        TelephonyManager tm = (TelephonyManager)
                activity.getSystemService(Context.TELEPHONY_SERVICE);

        if (sub_type == TelephonyManager.NETWORK_TYPE_LTE) {
          return 5; // 4G
        } else if (sub_type == TelephonyManager.NETWORK_TYPE_UMTS ||
                sub_type == TelephonyManager.NETWORK_TYPE_HSDPA ||
                sub_type == TelephonyManager.NETWORK_TYPE_EVDO_0) {
          return 5; // 3G
        } else if (sub_type == TelephonyManager.NETWORK_TYPE_GPRS ||
                sub_type == TelephonyManager.NETWORK_TYPE_EDGE ||
                sub_type == TelephonyManager.NETWORK_TYPE_CDMA) {
          return 4; // 2G
        } else {
          return 4; // 2G
        }
      }
    }
    return 0;
  }
  
  public static boolean is_ac_power() {
    if ( is_battery() ) {
      switch ( battery_status ) {
        case BatteryManager.BATTERY_STATUS_CHARGING: return true;
        case BatteryManager.BATTERY_STATUS_DISCHARGING: return false;
        case BatteryManager.BATTERY_STATUS_NOT_CHARGING:
        case BatteryManager.BATTERY_STATUS_FULL:
        case BatteryManager.BATTERY_STATUS_UNKNOWN: break;
      }
      return is_ac_power_connected;
    } else { // no battery
      return true;
    }
  }
  
  public static boolean is_battery() {
    BatteryManager bm =(BatteryManager)activity.getSystemService(Context.BATTERY_SERVICE);
    int capacity = bm.getIntProperty(BatteryManager.BATTERY_PROPERTY_CHARGE_COUNTER);
    return capacity > 0;
  }
  
  public static float battery_level() {
    if ( is_battery() ) {
      BatteryManager bm = (BatteryManager) activity.getSystemService(Context.BATTERY_SERVICE);
      float level = bm.getIntProperty(BatteryManager.BATTERY_PROPERTY_CAPACITY) / 100F;
      return level;
    } else {
      return 0;
    }
  }
  
  public static String language() {
    return Locale.getDefault().toString();
  }

  private static ActivityManager.MemoryInfo memory_info() {
    ActivityManager am = (ActivityManager)activity.getSystemService(Context.ACTIVITY_SERVICE);
    ActivityManager.MemoryInfo info = new ActivityManager.MemoryInfo();
    am.getMemoryInfo(info);
    return info;
  }

  public static long available_memory() {
    return memory_info().availMem;
  }

  public static long memory() {
    return memory_info().totalMem;
  }

  public static long used_memory() {
    ActivityManager am = (ActivityManager)activity.getSystemService(Context.ACTIVITY_SERVICE);
    List<ActivityManager.RunningAppProcessInfo> appProcessList = am.getRunningAppProcesses();
    int pid = android.os.Process.myPid();
    for (ActivityManager.RunningAppProcessInfo appProcessInfo : appProcessList) {
      if ( appProcessInfo.pid == pid ) {
        Debug.MemoryInfo[] memoryInfo = am.getProcessMemoryInfo(new int[] { pid });
        return memoryInfo[0].dalvikPrivateDirty * 1024;
      }
    }
    return 0;
  }

  static {
    System.loadLibrary("avocado");
  }
}
