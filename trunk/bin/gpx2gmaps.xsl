<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method = "text"  version="1.0"  />

<xsl:template match="/">
<xsl:apply-templates select="*[local-name()='gpx']"/>
</xsl:template>

<xsl:template match="*[local-name()='gpx']">&lt;!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"&gt;
&lt;html xmlns="http://www.w3.org/1999/xhtml" xmlns:v="urn:schemas-microsoft-com:vml"&gt;
&lt;head&gt;
&lt;meta http-equiv="content-type" content="text/html; charset=UTF-8"/&gt;
&lt;title&gt;Google Maps API Example - overlay&lt;/title&gt;
&lt;style type="text/css"&gt;
v\:* {
 behavior:url(#default#VML);
}
&lt;/style&gt;
&lt;script src="http://maps.google.com/maps?file=api&amp;v=1&amp;key=ABQIAAAA4Wxrd1ZmQfRHvggZWM0QkxSywvohUEBj468j1bHLctjAi9H1aRTgpH5EJsqp8F3DqOP3spOw36wc2A" type="text/javascript"&gt;&lt;/script&gt;
&lt;script type="text/javascript"&gt;
//&lt;![CDATA[
function onLoad() {
    var map = new GMap(document.getElementById("map"));
    map.setMapType(G_HYBRID_TYPE)
    map.addControl(new GSmallMapControl());
    map.addControl(new GMapTypeControl());
    var points = [];
        <xsl:apply-templates select="*[local-name()='trk']/*[local-name()='trkseg']/*[local-name()='trkpt']"/>
    map.addOverlay(new GPolyline(points));
    }
//]]&gt;
&lt;/script&gt;
&lt;/head&gt;
&lt;body onload="onLoad()"&gt;
&lt;div id="map" style="width: 500px; height: 300px"&gt;&lt;/div&gt;
&lt;div id="message"&gt;&lt;/div&gt;
&lt;/body&gt;
&lt;/html&gt;
</xsl:template>

<xsl:template match="*[local-name()='trk']/*[local-name()='trkseg']/*[local-name()='trkpt']">
<xsl:if test="position() = 1">
map.centerAndZoom(new GPoint(<xsl:value-of select="@lon"/>, <xsl:value-of select="@lat"/> ), 5);
</xsl:if>
points.push(new GPoint(<xsl:value-of select="@lon"/>, <xsl:value-of select="@lat"/> ));
</xsl:template>


</xsl:stylesheet>
