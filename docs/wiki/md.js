/* tiny markdown -> html (headers, bold, italic, code, links, lists, quotes, hr) */
function md(t){
  const esc=s=>s.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');
  const inline=s=>esc(s)
    .replace(/`([^`]+)`/g,'<code>$1</code>')
    .replace(/\*\*([^*]+)\*\*/g,'<b>$1</b>')
    .replace(/\*([^*]+)\*/g,'<i>$1</i>')
    .replace(/\[([^\]]+)\]\(([^)\s]+)\)/g,'<a href="$2">$1</a>');
  const lines=t.split('\n');let out='',inList=false,inQuote=false;
  const close=()=>{if(inList){out+='</ul>';inList=false}if(inQuote){out+='</blockquote>';inQuote=false}};
  for(const raw of lines){
    const l=raw.trimEnd();
    if(/^```/.test(l.trim())){close();out+='<pre><code>'+esc(l.trim().slice(3))+'</code></pre>';continue}
    if(!l.trim()){close();continue}
    let m;
    if(m=l.match(/^(#{1,4})\s+(.*)/)){close();const n=m[1].length;out+=`<h${n+1}>${inline(m[2])}</h${n+1}>`;continue}
    if(m=l.match(/^&gt;\s?(.*)/)){if(!inQuote){out+='<blockquote>';inQuote=true}out+='<p>'+inline(m[1])+'</p>';continue}
    if(m=l.match(/^[-*]\s+(.*)/)){if(!inList){out+='<ul>';inList=true}out+='<li>'+inline(m[1])+'</li>';continue}
    if(m=l.match(/^\d+[.)]\s+(.*)/)){if(!inList){out+='<ol>';inList=true}out+='<li>'+inline(m[1])+'</li>';continue}
    if(/^---+$/.test(l.trim())){close();out+='<hr>';continue}
    close();out+='<p>'+inline(l)+'</p>';
  }
  close();return out;
}
