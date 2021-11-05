#include <glib.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>

#include <QApplication>
#include <QTimer>
#include <QWidget>


int main(int argc, char *argv[])
{
  gst_init (&argc, &argv);
//  QApplication app(argc, argv);
//  app.setQuitOnLastWindowClosed(true);

  /* prepare the pipeline */

  GstElement *pipeline = gst_pipeline_new ("xvoverlay");
  GstElement *src = gst_element_factory_make ("v4l2src", NULL);
  GstElement *fps = gst_element_factory_make("videoconvert", NULL);
  GstElement *sink = gst_element_factory_make ("ximagesink", NULL);

  if (sink == NULL)
    g_error ("Couldn't find a working video sink.");

  gst_bin_add_many (GST_BIN (pipeline), src, fps, sink, NULL);
  gst_element_link_many (src, fps, sink, NULL);

  /* prepare the ui */

//  QWidget window;
//  window.resize(320, 240);
//  window.setWindowTitle("GstVideoOverlay Qt demo");
//  window.show();

//  WId xwinid = window.winId();
//  gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (sink), xwinid);

  /* run the pipeline */

  GstStateChangeReturn sret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  if (sret == GST_STATE_CHANGE_FAILURE) {
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
    /* Exit application */
//    QTimer::singleShot(0, QApplication::activeWindow(), SLOT(quit()));
    return -1;
  }

  /* Wait until error or EOS */
  GstBus *bus = gst_element_get_bus (pipeline);
  GstMessage *msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));


  /* Parse message */
    if (msg != NULL) {
      GError *err;
      gchar *debug_info;

      switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR:
          gst_message_parse_error (msg, &err, &debug_info);
          g_printerr ("Error received from element %s: %s\n",
              GST_OBJECT_NAME (msg->src), err->message);
          g_printerr ("Debugging information: %s\n",
              debug_info ? debug_info : "none");
          g_clear_error (&err);
          g_free (debug_info);
          break;
        case GST_MESSAGE_EOS:
          g_print ("End-Of-Stream reached.\n");
          break;
        default:
          /* We should not reach here because we only asked for ERRORs and EOS */
          g_printerr ("Unexpected message received.\n");
          break;
      }
      gst_message_unref (msg);
    }




//  int ret = app.exec();

//  window.hide();
  gst_object_unref(bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);


//  return ret;
}
