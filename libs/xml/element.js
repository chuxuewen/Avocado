/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the BSD license:
 *
 * Copyright (c) 2015, louis.chu
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of louis.chu nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL louis.chu BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * ***** END LICENSE BLOCK ***** */

import 'node';
import LiveNodeList from 'node';
import 'document';
import 'parser';

export class Element extends node.Node {

	nodeType: node.ELEMENT_NODE,
  
	hasAttribute (name) {
		return this.getAttributeNode(name) != null;
	}
  
	getAttribute (name) {
		var attr = this.getAttributeNode(name);
		return attr && attr.value || '';
	}

	setAttribute (name, value) {
		var attr = this.ownerDocument.createAttribute(name);
		attr.value = attr.nodeValue = value + '';
		this.setAttributeNode(attr);
	}

	getAttributeNode (name) {
		return this.attributes.getNamedItem(name);
	}

	setAttributeNode (newAttr) {
		this.attributes.setNamedItem(newAttr);
	}

	removeAttributeNode (oldAttr) {
		this.attributes._removeItem(oldAttr);
	}

	removeAttribute (name) {
		var attr = this.getAttributeNode(name);
		attr && this.removeAttributeNode(attr);
	}

	hasAttributeNS (namespaceURI, localName) {
		return this.getAttributeNodeNS(namespaceURI, localName) != null;
	}

	getAttributeNS (namespaceURI, localName) {
		var attr = this.getAttributeNodeNS(namespaceURI, localName);
		return attr && attr.value || '';
	}

	setAttributeNS (namespaceURI, qualifiedName, value) {
		var attr = this.ownerDocument.createAttributeNS(namespaceURI, qualifiedName);
		attr.value = attr.nodeValue = value + '';
		this.setAttributeNode(attr);
	}

	getAttributeNodeNS (namespaceURI, localName) {
		return this.attributes.getNamedItemNS(namespaceURI, localName);
	}

	setAttributeNodeNS (newAttr) {
		this.attributes.setNamedItemNS(newAttr);
	}

	removeAttributeNS (namespaceURI, localName) {
		var attr = this.getAttributeNodeNS(namespaceURI, localName);
		attr && this.removeAttributeNode(attr);
	}

	getElementsByTagName (name) {
		return new LiveNodeList(this, function (node) {
			var ls = [];
			document.visitNode(node, function (node) {
				if (node.nodeType == node.ELEMENT_NODE && node.tagName == name)
					ls.push(node);
				return true;
			});
			return ls;
		});
	}

	getElementsByTagNameNS (namespaceURI, localName) {
		return new LiveNodeList(this, function (node) {
			var ls = [];
			document.visitNode(node, function (node) {
				if (node.nodeType == node.ELEMENT_NODE && 
				node.namespaceURI == namespaceURI && 
				node.localName == localName)
					ls.push(node);
				return true;
			});
			return ls;
		});
	}

	get innerXml () {
		return Array.toArray(this.childNodes).join('');
	}

	set innerXml (xml) {

		this.removeAllChild();
		if(xml){
			new parser.Parser().fragment(this.ownerDocument, this, xml);
		}
	}

}
