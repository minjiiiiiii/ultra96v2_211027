#include <glib.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>

#include <QApplication>
#include <QTimer>
#include <QWidget>


int main(int argc, char *argv[])
{
  gst_init (&argc, &argv);
  QApplication app(argc, argv);
  app.setQuitOnLastWindowClosed(true);

  /* prepare the pipeline */

  GstElement *pipeline = gst_pipeline_new ("xvoverlay");
  GstElement *src = gst_element_factory_make ("v4l2src", NULL);
  GstElement *fps = gst_element_factory_make("videorate", NULL);
  GstElement *sink = gst_element_factory_make ("xvimagesink", NULL);

  if (sink == NULL)
    g_error ("Couldn't find a working video sink.");

  gst_bin_add_many (GST_BIN (pipeline), src, fps, sink, NULL);
  gst_element_link_many (src, fps, sink, NULL);

  /* prepare the ui */

  QWidget window;
  window.resize(320, 240);
  window.setWindowTitle("GstVideoOverlay Qt demo");
  window.show();

  WId xwinid = window.winId();
  gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (sink), xwinid);

  /* run the pipeline */

  GstStateChangeReturn sret = gst_element_set_state (pipeline,
      GST_STATE_PLAYING);
  if (sret == GST_STATE_CHANGE_FAILURE) {
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
    /* Exit application */
    QTimer::singleShot(0, QApplication::activeWindow(), SLOT(quit()));
  }

  int ret = app.exec();

  window.hide();
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);

  return ret;
}
