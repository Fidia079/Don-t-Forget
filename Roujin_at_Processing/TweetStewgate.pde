import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import java.util.List;

class TweetStewgate {
  String token;
  TweetStewgate(String _token) {
    token = _token;
  }
  
  void tweetMsg(String msg) {
    println("Tweet msg : " + msg);
    String url = "http://stewgate-u.appspot.com/api/post/";
    try {
        DefaultHttpClient httpClient = new DefaultHttpClient();
        HttpPost httpPost = new HttpPost( url );
        List nameValuePairs = new ArrayList(2);
        nameValuePairs.add(new BasicNameValuePair("_t", token));
        nameValuePairs.add(new BasicNameValuePair("msg", msg));
        httpPost.setEntity(new UrlEncodedFormEntity(nameValuePairs));
        println( "executing request: " + httpPost.getRequestLine() );
        HttpResponse response = httpClient.execute( httpPost );
        HttpEntity entity = response.getEntity();
        println( response.getStatusLine() );
        if ( entity != null ) entity.writeTo( System.out );
        if ( entity != null ) entity.consumeContent();
        httpClient.getConnectionManager().shutdown();
        }
        catch( Exception e ){
          e.printStackTrace();
        }
      }
}
